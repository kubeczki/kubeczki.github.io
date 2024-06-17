let viewportWidth;
let viewportHeight;
let viewportSize;

const memory = new WebAssembly.Memory({
	initial: 1024,  // Initial size in 64KB pages (1024 * 64KB = 64MB)
	maximum: 1024  // Maximum size in 64KB pages (optional)
});
let isApplicationFocused = !document.hidden;
	
// functions exported from JS to WASM
function callWindowAlert() {
	alert('TESTING');
}
function getMemoryCapacity() {
	return memory.buffer.byteLength;
}
function getIsApplicationFocused() {
	return isApplicationFocused;
}

async function init() {

	const { instance } = await WebAssembly.instantiateStreaming(
		fetch("game.wasm"),
		{
			env: {
				memory: memory,
				callWindowAlert,
				getMemoryCapacity,
				getIsApplicationFocused,
			}
		}
	);
	const fromC = instance.exports;
	// TODO: check if any exports.* isn't undefined

	const memoryView = new Uint8Array(instance.exports.memory.buffer);
	const capacity = instance.exports.memory.buffer.byteLength;
	
	let ratio = window.devicePixelRatio;
	const maxViewportWidth = fromC.getMaxViewportWidth();
	const maxViewportHeight = fromC.getMaxViewportHeight();
	
	const gameCanvas = document.getElementById("game-canvas");
	const canvasContext = gameCanvas.getContext('2d');
	// TODO: look at all these options 
	// https://developer.mozilla.org/en-US/docs/Web/API/HTMLCanvasElement/getContext

	viewportWidth = Math.ceil(Math.min(window.innerWidth, maxViewportWidth) * ratio);
    viewportHeight = Math.ceil(Math.min(window.innerHeight, maxViewportHeight) * ratio);
    viewportSize = viewportWidth * viewportHeight;
	fromC.setViewportDimensions(viewportWidth, viewportHeight, viewportSize);
	gameCanvas.width = viewportWidth;
	gameCanvas.height = viewportHeight;
	gameCanvas.style.width = Math.min(window.innerWidth, maxViewportWidth) + "px";
    gameCanvas.style.height = Math.min(window.innerHeight, maxViewportHeight) + "px";

	const inputAddress = fromC.getInputAddress();
	const buttonStateSize = fromC.getButtonStateSize();
	const inputSize = fromC.getInputSize();
	const keyboardInputSize = fromC.getKeyboardInputSize();
	const inputView = memoryView.subarray(
		inputAddress, 
		inputAddress + inputSize
		);
	const mouseInputView = memoryView.subarray(
		inputAddress + keyboardInputSize, 
		inputAddress + inputSize
		);
	const oldInput = new Uint8Array(inputSize);

	const inputKeyToIndex = {
		'ArrowLeft': 	0,
		'ArrowUp': 		1,
		'ArrowRight': 	2,
		'ArrowDown': 	3,
		'Space': 		4,
		'Enter': 		5,
		'Tab': 			6,
		'Escape': 		7,
		'Digit1': 		8,
		'Digit2': 		9,
		'Digit3': 		10,
		'Digit4': 		11,
		'Digit5': 		12,
		'Digit6': 		13,
		'Digit7': 		14,
		'Digit8': 		15,
		'Digit9': 		16,
		'Digit0': 		17,
		'ShiftRight': 	18,
		'ShiftLeft': 	18,
	};

	function updateKeyDownState(index) {
		inputView[index*buttonStateSize] = 1;
	}
	function updateKeyUpState(index) {
		inputView[index*buttonStateSize] = 0;
	}
	function updateInputState() {
		for (let index = 0; index < 18; index = index + 1) {
			inputView[index*buttonStateSize + 1] = 
				(!inputView[index*buttonStateSize + 1] && !oldInput[index*buttonStateSize] && inputView[index*buttonStateSize]) ? 1 : 0;
		}
	}

	document.addEventListener("visibilitychange", () => {
		if (document.hidden) fromC.pauseGame();
		console.log(!document.hidden ? 'App has just gained focus' : 'App has just lost focus');
	});
    document.addEventListener('keydown', event => {
	   	if (event.code === 'Tab' || 
			event.code === 'ArrowUp' || 
			event.code === 'ArrowDown' || 
			event.code === 'ArrowLeft' || 
			event.code === 'ArrowRight' ||
			event.code === 'Space')
			// TODO: any other we definitely want to ignore?
		{
            event.preventDefault();
        }
		updateKeyDownState(inputKeyToIndex[event.code]);
    });
	document.addEventListener('keyup', event => {
	   	if (event.code === 'Tab' || 
			event.code === 'ArrowUp' || 
			event.code === 'ArrowDown' || 
			event.code === 'ArrowLeft' || 
			event.code === 'ArrowRight' ||
			event.code === 'Space')
			// TODO: any other we definitely want to ignore?
		{
            event.preventDefault();
        }
		updateKeyUpState(inputKeyToIndex[event.code]);
    });

	function updateMouseInputState() {
		for (let index = 0; index < 3; index = index + 1) {
			mouseInputView[index*buttonStateSize + 1] = 
					(!mouseInputView[index*buttonStateSize + 1] && !oldInput[index*buttonStateSize] && mouseInputView[index*buttonStateSize]) ? 1 : 0;
		}
	}

    gameCanvas.addEventListener('pointermove', e => {
        fromC.mouseMove((e.offsetX * ratio), (e.offsetY * ratio));
    });
    gameCanvas.addEventListener('pointerdown', e => {
		// button: 0 - lmb, 1 - mmb, 2 - rmb
		mouseInputView[e.button*buttonStateSize] = 1;
    });
	gameCanvas.addEventListener('pointerup', e => {
		mouseInputView[e.button*buttonStateSize] = 0;
    });
	gameCanvas.addEventListener('wheel', e => {
		e.preventDefault();
		if (e.deltaY < 0){
			fromC.mouseScrollWheelUp();
		}
		if (e.deltaY > 0){
			fromC.mouseScrollWheelDown();
		}
	});

	fromC.init();
	const framebufferAddr = fromC.getFramebufferAddr();

	let frame = new ImageData(viewportWidth, viewportHeight);

	window.addEventListener("resize", e => {
		viewportWidth = Math.ceil(Math.min(e.target.innerWidth, maxViewportWidth) * ratio);
		viewportHeight = Math.ceil(Math.min(e.target.innerHeight, maxViewportHeight) * ratio);
		viewportSize = viewportWidth * viewportHeight;
		fromC.setViewportDimensions(viewportWidth, viewportHeight, viewportSize);
		gameCanvas.width = viewportWidth;
		gameCanvas.height = viewportHeight;
		gameCanvas.style.width = Math.min(e.target.innerWidth, maxViewportWidth) + "px";
	    gameCanvas.style.height = Math.min(e.target.innerHeight, maxViewportHeight) + "px";
		frame = new ImageData(viewportWidth, viewportHeight);
	});

	let start = performance.now();
	let timestamp = 0;
	let dt = 0;
	const targetFrameDuration = 16.67;
	/*
	while (true) {
		timestamp = performance.now();
		dt = (timestamp - start);
        start = timestamp;

		updateInputState();
		memoryView.set(input, inputAddress);
		oldInput.set(input);
		fromC.doFrame(dt);
		
		const frameData = memoryView.subarray(
			framebufferAddr, 
			framebufferAddr + 4 * viewportSize
			);
		frame.data.set(frameData);
		canvasContext.putImageData(frame, 0, 0);

		const delta = targetFrameDuration - (performance.now() - timestamp);
		if (delta > 0) await new Promise(r => setTimeout(r, 16-dt));
	}
	*/
	// NOTE: requestAnimationFrame does vSync!
	function step(timestamp) {
        const dt = (timestamp - start);
        start = timestamp;

		updateInputState();
		updateMouseInputState();
		memoryView.set(inputView, inputAddress);
		oldInput.set(inputView);
		fromC.doFrame(dt);
		
		const frameData = memoryView.subarray(
			framebufferAddr, 
			framebufferAddr + 4 * viewportSize
			);
		frame.data.set(frameData);
		canvasContext.putImageData(frame, 0, 0);
        
		window.requestAnimationFrame(step);
    }
    window.requestAnimationFrame(step);
}
init();