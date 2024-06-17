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
	const inputSize = fromC.getInputSize();
	const buttonStateSize = fromC.getButtonStateSize();
	const input = memoryView.subarray(
		inputAddress, 
		inputAddress + inputSize
		);
	const oldInput = new Uint8Array(inputSize);

	const inputData = {
		keyboard: {
			buttons: new Uint8Array(8),
			shift: 0,
			// ... other keyboard button properties
		},
		mouse: {
			buttons: new Uint8Array(3),
			// ... other mouse button properties
		}
	};

	function updateKeyDownState(index) {
		input[index*buttonStateSize] = 1;
	}
	function updateKeyUpState(index) {
		input[index*buttonStateSize] = 0;
	}
	function updateInputState() {
		for (let index = 0; index < 8; index = index + 1) {
			input[index*buttonStateSize + 1] = 
				(!input[index*buttonStateSize + 1] && !oldInput[index*buttonStateSize] && input[index*buttonStateSize]) ? 1 : 0;
		}
	}

	document.addEventListener("visibilitychange", () => {
		if (document.hidden) fromC.pauseGame();
		console.log(!document.hidden ? 'App has just gained focus' : 'App has just lost focus');
	});
    document.addEventListener('keydown', event => {
		// TODO: keyCode is deprecated, if anything, you should
		// unfortunately use code
	   	if (event.key === 'Tab' || 
			event.key === 'ArrowUp' || 
			event.key === 'ArrowDown' || 
			event.key === 'ArrowLeft' || 
			event.key === 'ArrowRight' ||
			event.key === 'Space')
			// TODO: any other we definitely want to ignore?
		{
            event.preventDefault();
        }
		switch (event.code) {
			case ("ArrowLeft"): {
				updateKeyDownState(0);
			} break;
			case ("ArrowUp"): {
				updateKeyDownState(1);
			} break;
			case ("ArrowRight"): {
				updateKeyDownState(2);
			} break;
			case ("ArrowDown"): {
				updateKeyDownState(3);
			} break;
			case ("Space"): {
				updateKeyDownState(4);
			} break;
			case ("Enter"): {
				updateKeyDownState(5);
			} break;
			case ("Tab"): {
				updateKeyDownState(6);
			} break;
			case ("Escape"): {
				updateKeyDownState(7);
			} break;
			case ("Digit1"): {
				updateKeyDownState(8)
			} break;
			case ("Digit2"): {
				updateKeyDownState(9)
			} break;
			case ("Digit3"): {
				updateKeyDownState(10)
			} break;
			case ("Digit4"): {
				updateKeyDownState(11)
			} break;
			case ("Digit5"): {
				updateKeyDownState(12)
			} break;
			case ("Digit6"): {
				updateKeyDownState(13)
			} break;
			case ("Digit7"): {
				updateKeyDownState(14)
			} break;
			case ("Digit8"): {
				updateKeyDownState(15)
			} break;
			case ("Digit9"): {
				updateKeyDownState(16)
			} break;
			case ("Digit0"): {
				updateKeyDownState(17)
			} break;
			case ("ShiftRight"):
			case ("ShiftLeft"): {
				updateKeyDownState(18);
			} break;
		}
    });
	document.addEventListener('keyup', event => {
		// TODO: keyCode is deprecated, if anything, you should
		// unfortunately use code
	   	if (event.key === 'Tab' || 
			event.key === 'ArrowUp' || 
			event.key === 'ArrowDown' || 
			event.key === 'ArrowLeft' || 
			event.key === 'ArrowRight' ||
			event.key === 'Space')
			// TODO: any other we definitely want to ignore?
		{
            event.preventDefault();
        }
		
		switch (event.code) {
			case ("ArrowLeft"): {
				updateKeyUpState(0);
			} break;
			case ("ArrowUp"): {
				updateKeyUpState(1);
			} break;
			case ("ArrowRight"): {
				updateKeyUpState(2);
			} break;
			case ("ArrowDown"): {
				updateKeyUpState(3);
			} break;
			case ("Space"): {
				updateKeyUpState(4);
			} break;
			case ("Enter"): {
				updateKeyUpState(5);
			} break;
			case ("Tab"): {
				updateKeyUpState(6);
			} break;
			case ("Escape"): {
				updateKeyUpState(7);
			} break;
						case ("Digit1"): {
				updateKeyUpState(8)
			} break;
			case ("Digit2"): {
				updateKeyUpState(9)
			} break;
			case ("Digit3"): {
				updateKeyUpState(10)
			} break;
			case ("Digit4"): {
				updateKeyUpState(11)
			} break;
			case ("Digit5"): {
				updateKeyUpState(12)
			} break;
			case ("Digit6"): {
				updateKeyUpState(13)
			} break;
			case ("Digit7"): {
				updateKeyUpState(14)
			} break;
			case ("Digit8"): {
				updateKeyUpState(15)
			} break;
			case ("Digit9"): {
				updateKeyUpState(16)
			} break;
			case ("Digit0"): {
				updateKeyUpState(17)
			} break;
			case ("ShiftRight"):
			case ("ShiftLeft"): {
				updateKeyUpState(18);
			} break;
		}
    });
    gameCanvas.addEventListener('pointermove', e => {
        fromC.mouseMove((e.offsetX * ratio), (e.offsetY * ratio));
    });
    gameCanvas.addEventListener('pointerdown', e => {
        fromC.mouseDown();
    });
	gameCanvas.addEventListener('pointerup', e => {
        fromC.mouseUp();
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
		memoryView.set(input, inputAddress);
		oldInput.set(input);
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