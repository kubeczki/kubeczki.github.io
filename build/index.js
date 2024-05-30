let viewportWidth;
let viewportHeight;
let viewportSize;

/*
const jsArray = [1, 2, 3, 4, 5];
const cArrayPointer = instance.exports.myMalloc(jsArray.length * 4);
const cArray = new Uint32Array(
	instance.exports.memory.buffer,
	cArrayPointer,
	jsArray.length
);
cArray.set(jsArray);
*/

async function init() {
	
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
	
	// TODO: I THINK that if you want a function
	// that takes an address, then you have to do
	// that Memory thingy here, cause you need it
	// before instantiating the WASM instance

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
	// TODO: check if any exports.* isn't undefined

	const memoryView = new Uint8Array(instance.exports.memory.buffer);
	const capacity = instance.exports.memory.buffer.byteLength;
	// NOTE: I think that we could just have like 3 'things' that we 
	// share between JS and C - framebuffer memory and some structs
	// for things like input. The structs would be set up in the
	// very same way as framebuffer is already handled!
	
	let ratio = window.devicePixelRatio;
	const maxViewportWidth = instance.exports.getMaxViewportWidth();
	const maxViewportHeight = instance.exports.getMaxViewportHeight();
	
	const gameCanvas = document.getElementById("game-canvas");
	const canvasContext = gameCanvas.getContext('2d');
	// TODO: look at all these options 
	// https://developer.mozilla.org/en-US/docs/Web/API/HTMLCanvasElement/getContext
	// canvasContext.scale(ratio, ratio); // TODO: I _THINK_ that we don't need this

	viewportWidth = Math.ceil(Math.min(window.innerWidth, maxViewportWidth) * ratio);
    viewportHeight = Math.ceil(Math.min(window.innerHeight, maxViewportHeight) * ratio);
    viewportSize = viewportWidth * viewportHeight;
	instance.exports.setViewportDimensions(viewportWidth, viewportHeight, viewportSize);
	gameCanvas.width = viewportWidth;
	gameCanvas.height = viewportHeight;
	gameCanvas.style.width = Math.min(window.innerWidth, maxViewportWidth) + "px";
    gameCanvas.style.height = Math.min(window.innerHeight, maxViewportHeight) + "px";

	const inputAddress = instance.exports.getInputAddress();
	const inputSize = instance.exports.getInputSize();
	const buttonStateSize = instance.exports.getButtonStateSize();
	const input = memoryView.subarray(
		inputAddress, 
		inputAddress + inputSize
		);
	const oldInput = new Uint8Array(inputSize);

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
		if (document.hidden) instance.exports.pauseGame();
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
			case ("ShiftRight"):
			case ("ShiftLeft"): {
				updateKeyDownState(8);
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
			case ("ShiftRight"):
			case ("ShiftLeft"): {
				updateKeyUpState(8);
			} break;
		}
    });
    gameCanvas.addEventListener('mousemove', e => {
        instance.exports.mouseMove((e.offsetX * ratio), (e.offsetY * ratio));
    });
    gameCanvas.addEventListener('mousedown', e => {
        instance.exports.mouseDown();
    });
	gameCanvas.addEventListener('mouseup', e => {
        instance.exports.mouseUp();
    });

	instance.exports.init();
	const framebufferAddr = instance.exports.getFramebufferAddr();

	let frame = new ImageData(viewportWidth, viewportHeight);

	window.addEventListener("resize", e => {
		viewportWidth = Math.ceil(Math.min(e.target.innerWidth, maxViewportWidth) * ratio);
		viewportHeight = Math.ceil(Math.min(e.target.innerHeight, maxViewportHeight) * ratio);
		viewportSize = viewportWidth * viewportHeight;
		instance.exports.setViewportDimensions(viewportWidth, viewportHeight, viewportSize);
		gameCanvas.width = viewportWidth;
		gameCanvas.height = viewportHeight;
		gameCanvas.style.width = Math.min(e.target.innerWidth, maxViewportWidth) + "px";
	    gameCanvas.style.height = Math.min(e.target.innerHeight, maxViewportHeight) + "px";
		frame = new ImageData(viewportWidth, viewportHeight);
	});

	let start = document.timeline.currentTime;
	function step(timestamp) {
        const dt = (timestamp - start);
        start = timestamp;

		// TODO: share input queue with C in this place?
		// getInputQueue(inputStructPointer)
		updateInputState();
		memoryView.set(input, inputAddress);
		oldInput.set(input);
		instance.exports.doFrame(dt);
		
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