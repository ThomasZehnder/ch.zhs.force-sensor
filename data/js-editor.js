//development, set fix IP as URL, otherwise set empty
let devUrl = "";

// Get the current hostname
var currentHostname = window.location.hostname;
// Log the hostname to the console
console.log("Hostname: " + currentHostname);
if (currentHostname.startsWith("127") || currentHostname.startsWith("localhost")) {
    devUrl = "http://192.168.1.231";
}

const codeTextArea = document.getElementById('code');
const codeMirrorEditor = CodeMirror.fromTextArea(codeTextArea, {
    mode: 'javascript',
    theme: 'dracula', // Specify the dark theme here
    lineNumbers: true
});

const outputTextArea = document.getElementById('console');
outputTextArea.rows = 10;
outputTextArea.style.backgroundColor = "black";
outputTextArea.style.color = "white";


const runButton = document.getElementById('runButton');
const cyclicButton = document.getElementById('cyclicButton');
const clearLedButton = document.getElementById('clearLedButton');

const loadSample1 = document.getElementById('loadSample1');
const loadSample2 = document.getElementById('loadSample2');
const loadSample3 = document.getElementById('loadSample3');
const loadSample4 = document.getElementById('loadSample4');



//overwrite console.log()
const originalConsoleLog = console.log;

console.log = function (...args) {
    originalConsoleLog.apply(console, args);
    outputTextArea.value += args.map(arg => String(arg)).join(' ') + '\n';
    outputTextArea.scrollTop = outputTextArea.scrollHeight;
};

//buttons click events
loadSample1.addEventListener('click', async () => {
    //erase outputTextArea
    outputTextArea.value = ''; // Set the value to an empty string

    console.log("load Led Sample 1");
    const newContent = '\
Esp.clearLeds();\n\
Esp.setLed(3, "green"); \n \n\
//try other led number and colors: e.g red, white, magenta, black';

    codeMirrorEditor.getDoc().setValue(newContent);
    JS.cycleTime = 1000;
    startJsRunner();
});

loadSample2.addEventListener('click', async () => {
    //erase outputTextArea
    outputTextArea.value = ''; // Set the value to an empty string

    console.log("load Key Sample 2");
    const newContent = '\
if (Esp.getKey(1)==1)  //yellow key\n\
{\n\
  Esp.setLed(4, "magenta");\n\
} else {\n\
  Esp.setLed(4, "blue");\n\
};\n\
//try other key number and change colors: e.g red, white, magenta, black';

    codeMirrorEditor.getDoc().setValue(newContent);
    JS.cycleTime = 400;
    startJsRunner();
});

loadSample3.addEventListener('click', async () => {
    //erase outputTextArea
    outputTextArea.value = ''; // Set the value to an empty string

    console.log("load Blink Sample 3");
    const newContent = '\
if (toggle==1)\n\
{\n\
  toggle=0;\n\
  Esp.setLed(2, "red");\n\
} else {\n\
  toggle=1;\n\
  Esp.setLed(2, "green");\n\
};\n\
//add other leds and change colors: e.g red, white, magenta, black';

    codeMirrorEditor.getDoc().setValue(newContent);
    JS.cycleTime = 500;
    startJsRunner();
});

loadSample4.addEventListener('click', async () => {
    //erase outputTextArea
    outputTextArea.value = ''; // Set the value to an empty string

    console.log("load Night Rider Sample 4");
    const newContent = '\
i = i + 1;\n\
\n\
if (i > 6) {\n\
    i = 1;\n\
}\n\
\n\
Esp.clearLeds();\n\
Esp.setLed(i, "blue");\n\
\n\
//add other leds and change colors: e.g red, white, magenta, black';

    codeMirrorEditor.getDoc().setValue(newContent);
    JS.cycleTime = 500;
    startJsRunner();
});


clearLedButton.addEventListener('click', async () => {
    //erase outputTextArea
    outputTextArea.value = 'Esp.clearLeds();';
    fetchBlockedCounter = 0;

    const code = codeMirrorEditor.getValue();
    try {

        Esp.clearLeds();
        await sendColors(Esp.leds);
    } catch (error) {
        // Extract information about the error
        const errorMessage = error.message;
        const errorLine = error.lineNumber || error.line || error.stack.match(/<anonymous>:(\d+):\d+/)[1]; // Extract line number from stack trace
        const errorColumn = error.columnNumber || error.column;

        outputTextArea.value += `Error: ${errorMessage} at line ${errorLine}, column ${errorColumn}\n`;
        outputTextArea.scrollTop = outputTextArea.scrollHeight;
    }
});



runButton.addEventListener('click', async () => {
    //erase outputTextArea
    outputTextArea.value = 'Start JS Runner once....\n'; // Set the value to an empty string

    JS.cycleRun = false;
    updateCyclicButton();
    JS.init = true;
    runJS();
});



function updateCyclicButton() {
    if (JS.cycleRun) {
        cyclicButton.textContent = "Cyclic Stop";
        cyclicButton.style.backgroundColor = "lightcoral";
    } else {
        cyclicButton.textContent = "Cyclic Run Code";
        cyclicButton.style.backgroundColor = "lightgreen";
    }
}

function startJsRunner() {
    JS.cycleRun = true;
    updateCyclicButton();
    outputTextArea.value += 'Cyclic Run is started...\n';
    JS.init = true;
    runJS();
    clearTimeout(JS.timer);//in case still running, stop old timer
    JS.timer = setTimeout(cyclicJS, JS.cycleTime);
}

cyclicButton.addEventListener('click', async () => {
    //erase outputTextArea
    outputTextArea.value = '\n'; // Set the value to an empty string

    JS.cycleRun = !JS.cycleRun;
    updateCyclicButton();

    if (JS.cycleRun) {
        startJsRunner();
    } else {
        outputTextArea.value = 'Cyclic Run is stopped... \n\nStart again or select a sample.\n';
        clearTimeout(JS.timer);
    }
});


// led area
const circles = document.querySelectorAll('.circle');

// key area
const keys = document.querySelectorAll('.key');

//get color
const rgbValue = getColorRGBFromCSS("magenta");

function getColorRGBFromCSS(colorName) {
    const element = document.createElement("div");
    element.style.color = colorName;
    document.body.appendChild(element);

    const computedStyle = getComputedStyle(element);
    const colorValue = computedStyle.color;

    document.body.removeChild(element);


    // Convert colorValue to #RRGGBB notation
    if (colorValue.indexOf("rgb") === 0) {
        const rgbArray = colorValue.match(/\d+/g).map(Number);
        const hexValue = `#${rgbArray[0].toString(16).padStart(2, "0")}${rgbArray[1].toString(16).padStart(2, "0")}${rgbArray[2].toString(16).padStart(2, "0")}`;
        //console.log("getColorRGBFromCSS()", colorName, hexValue);
        return hexValue;
    }
    //console.log("getColorRGBFromCSS()", colorName, colorValue);
    return colorValue;
}

//esp functions
let Esp = {};
Esp.keys = [0, 1, 0];
Esp.leds = ["#000000", "#000000", "#000000", "#000000", "#000000", "#000000", "#000000", "#000000"];

Esp.getKey = (idx) => {
    let value = Esp.keys[idx];
    //console.log("Esp.getKey()", idx, value);
    return value;
}

Esp.setLed = (idx, color) => {
    let rgbColor = "";
    if (color.startsWith('#')) {
        rgbColor = color;
    } else {
        rgbColor = getColorRGBFromCSS(color);
    }

    //console.log("Esp.setLed()", idx, rgbColor);
    Esp.leds[idx] = rgbColor;
    circles[idx].style.backgroundColor = rgbColor;
}
Esp.clearLeds = () => {
    Esp.leds = ["#000000", "#000000", "#000000", "#000000", "#000000", "#000000", "#000000", "#000000"];

    circles.forEach((circle, idx) => {
        circles[idx].style.backgroundColor = Esp.leds[idx];
    });
}


//JS Parameter
let JS = {};
JS.cycleTime = 500;
JS.cycleRun = false;
JS.init = true;
JS.toggle = false;
JS.i = 0;
var toggle = false;
var i = 0;
var j = 0;
var color = "black";

JS.setCycleTime = (cycleTime) => {

    JS.cycleTime = cycleTime;
    //console.log("setCycleTime()", cycleTime);
}

function cyclicJS() {
    if (JS.cycleRun) {
        JS.init = false;
        runJS();
        //start again
        JS.timer = setTimeout(cyclicJS, JS.cycleTime);
    }
}

async function runJS() {
    const code = codeMirrorEditor.getValue();
    try {
        await receiveKeys();
        eval(code);
        await sendColors(Esp.leds);
    } catch (error) {
        // Extract information about the error
        const errorMessage = error.message;
        const errorLine = error.lineNumber || error.line || error.stack.match(/<anonymous>:(\d+):\d+/)[1]; // Extract line number from stack trace
        const errorColumn = error.columnNumber || error.column;

        outputTextArea.value += `\nError: ${errorMessage} at line ${errorLine}, column ${errorColumn}\n\n`;
        outputTextArea.value += '--> Try again, press "Cycle Run Code" button.\n';
        outputTextArea.scrollTop = outputTextArea.scrollHeight;

        JS.cycleRun = false;
        updateCyclicButton();
    }
}

let fetchSemaphore = false;
let fetchBlockedCounter = 0;

//REST Services
async function sendColors(colors) {

    if (fetchSemaphore) {
        fetchBlockedCounter++;
        console.log("fetch sendColors() blocked... ", fetchBlockedCounter);
        return;
    }

    let hexColors = "";

    colors.forEach((color) => {
        hexColors += color + ",";
    });

    let encodedColors = hexColors.replace(/#/g, '%23');

    const url = `${devUrl}/setleds?colors=${encodedColors}`;

    try {
        fetchSemaphore = true;
        const response = await fetch(url);
        if (response.ok) {
            //console.log(`Color set to ${encodedColors}`);
            fetchSemaphore = false;
        } else {
            console.log(`Failed to set color: ${response.statusText}`);
            fetchSemaphore = false;
        }
    } catch (error) {
        console.log(`Error sending sendColors() request: ${error}`);
        fetchSemaphore = false;
    }
}

async function receiveKeys() {

    if (fetchSemaphore) {
        fetchBlockedCounter++;
        console.log("fetch receiveKeys() blocked... ", fetchBlockedCounter);
        return;
    }
    const url = `${devUrl}/getkeys`;

    //console.log('call url: ', url);

    try {
        fetchSemaphore = true;
        const response = await fetch(url);
        if (response.ok) {
            const data = await response.json(); // Parse the response as JSON
            //console.log('Response data as JSON: ', data);
            Esp.keys[0] = data.key_1 ? 1 : 0;
            Esp.keys[1] = data.key_2 ? 1 : 0;
            Esp.keys[2] = data.key_3 ? 1 : 0;

            keys.forEach((key, idx) => {
                key.style.backgroundColor = Esp.keys[idx] == 1 ? "magenta" : "black";
            });
            fetchSemaphore = false;
        } else {
            console.log(`Failed to get keys: ${response.statusText}`);
            fetchSemaphore = false;
        }
    } catch (error) {
        console.log(`Error sending receiveKeys() request: ${error}`);
        fetchSemaphore = false;
    }
}