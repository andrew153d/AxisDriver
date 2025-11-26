let port;
let buffer = new Uint8Array();

// Initialize status
document.addEventListener('DOMContentLoaded', function() {
    const statusElement = document.getElementById('status');
    const indicatorElement = document.getElementById('connectionIndicator');
    statusElement.className = 'disconnected';
    indicatorElement.className = 'connection-indicator disconnected';
});

document.getElementById('connect').addEventListener('click', function() {
    console.log('Connect button clicked');
    connect();
});

// Basic Configuration
document.getElementById('getVersion').addEventListener('click', () => sendMessage(buildGetVersion()));
document.getElementById('setI2CAddress').addEventListener('click', setI2CAddress);
document.getElementById('getI2CAddress').addEventListener('click', () => sendMessage(buildGetI2CAddress()));
document.getElementById('setEthernetAddress').addEventListener('click', setEthernetAddress);
document.getElementById('getEthernetAddress').addEventListener('click', () => sendMessage(buildGetEthernetAddress()));
document.getElementById('setEthernetPort').addEventListener('click', setEthernetPort);
document.getElementById('getEthernetPort').addEventListener('click', () => sendMessage(buildGetEthernetPort()));
document.getElementById('getMacAddress').addEventListener('click', () => sendMessage(buildGetMacAddress()));
document.getElementById('saveConfiguration').addEventListener('click', () => sendMessage(buildSaveConfiguration()));

// LED Control
document.getElementById('setLedColor').addEventListener('click', setLedColor);
document.getElementById('getLedColor').addEventListener('click', () => sendMessage(buildGetLedColor()));
document.getElementById('addLedStep').addEventListener('click', addLedStep);

// Home Configuration
document.getElementById('setHomeDirection').addEventListener('click', setHomeDirection);
document.getElementById('getHomeDirection').addEventListener('click', () => sendMessage(buildGetHomeDirection()));
document.getElementById('setHomeThreshold').addEventListener('click', setHomeThreshold);
document.getElementById('getHomeThreshold').addEventListener('click', () => sendMessage(buildGetHomeThreshold()));
document.getElementById('setHomeSpeed').addEventListener('click', setHomeSpeed);
document.getElementById('getHomeSpeed').addEventListener('click', () => sendMessage(buildGetHomeSpeed()));
document.getElementById('getHomedState').addEventListener('click', () => sendMessage(buildGetHomedState()));
document.getElementById('home').addEventListener('click', home);

// Motor Control
document.getElementById('setMotorState').addEventListener('click', setMotorState);
document.getElementById('getMotorState').addEventListener('click', () => sendMessage(buildGetMotorState()));
document.getElementById('setMotorBrake').addEventListener('click', setMotorBrake);
document.getElementById('getMotorBrake').addEventListener('click', () => sendMessage(buildGetMotorBrake()));
document.getElementById('setMaxSpeed').addEventListener('click', setMaxSpeed);
document.getElementById('getMaxSpeed').addEventListener('click', () => sendMessage(buildGetMaxSpeed()));
document.getElementById('setAcceleration').addEventListener('click', setAcceleration);
document.getElementById('getAcceleration').addEventListener('click', () => sendMessage(buildGetAcceleration()));
document.getElementById('setCurrentPosition').addEventListener('click', setCurrentPosition);
document.getElementById('getCurrentPosition').addEventListener('click', () => sendMessage(buildGetCurrentPosition()));
document.getElementById('setTargetPosition').addEventListener('click', setTargetPosition);
document.getElementById('getTargetPosition').addEventListener('click', () => sendMessage(buildGetTargetPosition()));
document.getElementById('setRelativeTargetPosition').addEventListener('click', setRelativeTargetPosition);
document.getElementById('setVelocity').addEventListener('click', setVelocity);
document.getElementById('getVelocity').addEventListener('click', () => sendMessage(buildGetVelocity()));
document.getElementById('setVelocityAndSteps').addEventListener('click', setVelocityAndSteps);
document.getElementById('startPath').addEventListener('click', startPath);

async function connect() {
    const statusElement = document.getElementById('status');
    const indicatorElement = document.getElementById('connectionIndicator');
    
    // Check if Web Serial API is supported
    if (!('serial' in navigator)) {
        alert('Web Serial API not supported. Please use Chrome/Edge 89+ and ensure the page is served over HTTPS.');
        statusElement.textContent = 'Web Serial not supported';
        statusElement.className = 'disconnected';
        indicatorElement.className = 'connection-indicator disconnected';
        return;
    }
    
    statusElement.textContent = 'Connecting...';
    statusElement.className = 'connecting';
    indicatorElement.className = 'connection-indicator connecting';

    try {
        port = await navigator.serial.requestPort();
        await port.open({ baudRate: 115200 });
        statusElement.textContent = 'Connected';
        statusElement.className = 'connected';
        indicatorElement.className = 'connection-indicator connected';
        const reader = port.readable.getReader();
        readLoop(reader);
    } catch (error) {
        console.error('Error connecting:', error);
        statusElement.textContent = 'Connection failed: ' + error.message;
        statusElement.className = 'disconnected';
        indicatorElement.className = 'connection-indicator disconnected';
    }
}

async function readLoop(reader) {
    try {
        while (true) {
            const { value, done } = await reader.read();
            if (done) break;
            processData(value);
        }
    } catch (error) {
        console.error('Read error:', error);
        // Update connection status
        const statusElement = document.getElementById('status');
        const indicatorElement = document.getElementById('connectionIndicator');
        statusElement.textContent = 'Connection lost';
        statusElement.className = 'disconnected';
        indicatorElement.className = 'connection-indicator disconnected';
    }
}

function processData(data) {
    buffer = new Uint8Array([...buffer, ...data]);
    while (buffer.length >= 6) {
        const view = new DataView(buffer.buffer.slice(buffer.byteOffset, buffer.byteOffset + buffer.length));
        const message_type = view.getUint16(0, true);
        const body_size = view.getUint16(2, true);
        const total_size = 4 + body_size + 2;
        if (buffer.length < total_size) break;
        const message = buffer.slice(0, total_size);
        buffer = buffer.slice(total_size);
        handleMessage(message);
    }
}

function handleMessage(msg) {
    const view = new DataView(msg.buffer);
    const message_type = view.getUint16(0, true);
    const body_size = view.getUint16(2, true);
    switch (message_type) {
        case MESSAGE_TYPES.AckId:
            const ack = parseAck(msg);
            console.log('ACK for', ack.ackType.toString(16), 'status', ack.status);
            break;
        case MESSAGE_TYPES.GetVersionId:
            const ver = parseGetVersion(msg);
            document.getElementById('version').textContent = ver.version;
            break;
        case MESSAGE_TYPES.GetI2CAddressId:
            const i2c = parseGetI2CAddress(msg);
            document.getElementById('currentI2CAddress').textContent = i2c.address;
            break;
        case MESSAGE_TYPES.GetEthernetAddressId:
            const eth = parseGetEthernetAddress(msg);
            document.getElementById('currentEthernetAddress').textContent = ipToString(eth.ipAddress);
            break;
        case MESSAGE_TYPES.GetEthernetPortId:
            const port = parseGetEthernetPort(msg);
            document.getElementById('currentEthernetPort').textContent = port.port;
            break;
        case MESSAGE_TYPES.GetMacAddressId:
            const mac = parseGetMacAddress(msg);
            document.getElementById('currentMacAddress').textContent = mac.mac.map(b => b.toString(16).padStart(2, '0')).join(':');
            break;
        case MESSAGE_TYPES.GetLedColorId:
            const color = parseGetLedColor(msg);
            document.getElementById('currentLedColor').textContent = `R:${color.r} G:${color.g} B:${color.b}`;
            break;
        case MESSAGE_TYPES.GetHomeDirectionId:
            const dir = parseGetHomeDirection(msg);
            document.getElementById('currentHomeDirection').textContent = dir.direction === 0 ? 'Clockwise' : 'Counterclockwise';
            break;
        case MESSAGE_TYPES.GetHomeThresholdId:
            const thresh = parseGetHomeThreshold(msg);
            document.getElementById('currentHomeThreshold').textContent = thresh.threshold;
            break;
        case MESSAGE_TYPES.GetHomeSpeedId:
            const hspeed = parseGetHomeSpeed(msg);
            document.getElementById('currentHomeSpeed').textContent = hspeed.speed;
            break;
        case MESSAGE_TYPES.GetHomedStateId:
            const homed = parseGetHomedState(msg);
            document.getElementById('currentHomedState').textContent = homed.homed ? 'Homed' : 'Not Homed';
            break;
        case MESSAGE_TYPES.GetMotorStateId:
            const mstate = parseGetMotorState(msg);
            const stateNames = ['OFF', 'POSITION', 'VELOCITY', 'VELOCITY_STEP', 'IDLE_ON', 'HOME'];
            document.getElementById('currentMotorState').textContent = stateNames[mstate.motorState] || 'Unknown';
            break;
        case MESSAGE_TYPES.GetMotorBrakeId:
            const brake = parseGetMotorBrake(msg);
            const brakeNames = ['NORMAL', 'FREEWHEELING', 'STRONG_BRAKING', 'BRAKING'];
            document.getElementById('currentMotorBrake').textContent = brakeNames[brake.brakeMode] || 'Unknown';
            break;
        case MESSAGE_TYPES.GetMaxSpeedId:
            const maxspd = parseGetMaxSpeed(msg);
            document.getElementById('currentMaxSpeed').textContent = maxspd.maxSpeed;
            break;
        case MESSAGE_TYPES.GetAccelerationId:
            const accel = parseGetAcceleration(msg);
            document.getElementById('currentAcceleration').textContent = accel.acceleration;
            break;
        case MESSAGE_TYPES.GetCurrentPositionId:
            const cpos = parseGetCurrentPosition(msg);
            document.getElementById('currentPositionValue').textContent = cpos.position.toFixed(2);
            break;
        case MESSAGE_TYPES.GetTargetPositionId:
            const tpos = parseGetTargetPosition(msg);
            document.getElementById('currentTargetPosition').textContent = tpos.position.toFixed(2);
            break;
        case MESSAGE_TYPES.GetVelocityId:
            const vel = parseGetVelocity(msg);
            document.getElementById('currentVelocity').textContent = vel.velocity.toFixed(2);
            break;
        default:
            console.log('Unknown message type:', message_type.toString(16));
    }
}

async function sendMessage(msg) {
    if (!port) return;
    try {
        const writer = port.writable.getWriter();
        await writer.write(msg);
        writer.releaseLock();
    } catch (error) {
        console.error('Send error:', error);
    }
}

// Utility functions
function ipToString(ip) {
    return [
        (ip >>> 24) & 0xFF,
        (ip >>> 16) & 0xFF,
        (ip >>> 8) & 0xFF,
        ip & 0xFF
    ].join('.');
}

function stringToIp(ipString) {
    const parts = ipString.split('.').map(Number);
    return (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];
}

// Handler functions
function setI2CAddress() {
    const address = parseInt(document.getElementById('i2cAddress').value);
    const msg = buildSetI2CAddress(address);
    sendMessage(msg);
}

function setEthernetAddress() {
    const ipString = document.getElementById('ethernetAddress').value;
    const ip = stringToIp(ipString);
    const msg = buildSetEthernetAddress(ip);
    sendMessage(msg);
}

function setEthernetPort() {
    const port = parseInt(document.getElementById('ethernetPort').value);
    const msg = buildSetEthernetPort(port);
    sendMessage(msg);
}

function setLedColor() {
    const r = parseInt(document.getElementById('ledR').value);
    const g = parseInt(document.getElementById('ledG').value);
    const b = parseInt(document.getElementById('ledB').value);
    const msg = buildSetLedColor(r, g, b);
    sendMessage(msg);
}

function addLedStep() {
    const timeMs = parseInt(document.getElementById('ledStepTime').value);
    const r = parseInt(document.getElementById('ledStepR').value);
    const g = parseInt(document.getElementById('ledStepG').value);
    const b = parseInt(document.getElementById('ledStepB').value);
    const msg = buildAddLedStep(timeMs, r, g, b);
    sendMessage(msg);
}

function setHomeDirection() {
    const direction = parseInt(document.getElementById('homeDirection').value);
    const msg = buildSetHomeDirection(direction);
    sendMessage(msg);
}

function setHomeThreshold() {
    const threshold = parseInt(document.getElementById('homeThreshold').value);
    const msg = buildSetHomeThreshold(threshold);
    sendMessage(msg);
}

function setHomeSpeed() {
    const speed = parseInt(document.getElementById('homeSpeed').value);
    const msg = buildSetHomeSpeed(speed);
    sendMessage(msg);
}

function home() {
    const command = parseInt(document.getElementById('homeCommand').value);
    const msg = buildHome(command);
    sendMessage(msg);
}

function setMotorState() {
    const state = parseInt(document.getElementById('motorState').value);
    const msg = buildSetMotorState(state);
    sendMessage(msg);
}

function setMotorBrake() {
    const brake = parseInt(document.getElementById('motorBrake').value);
    const msg = buildSetMotorBrake(brake);
    sendMessage(msg);
}

function setMaxSpeed() {
    const speed = parseInt(document.getElementById('maxSpeed').value);
    const msg = buildSetMaxSpeed(speed);
    sendMessage(msg);
}

function setAcceleration() {
    const accel = parseInt(document.getElementById('acceleration').value);
    const msg = buildSetAcceleration(accel);
    sendMessage(msg);
}

function setCurrentPosition() {
    const position = parseFloat(document.getElementById('currentPosition').value);
    const msg = buildSetCurrentPosition(position);
    sendMessage(msg);
}

function setTargetPosition() {
    const position = parseFloat(document.getElementById('targetPosition').value);
    const msg = buildSetTargetPosition(position);
    sendMessage(msg);
}

function setRelativeTargetPosition() {
    const position = parseFloat(document.getElementById('relativeTargetPosition').value);
    const msg = buildSetRelativeTargetPosition(position);
    sendMessage(msg);
}

function setVelocity() {
    const velocity = parseFloat(document.getElementById('velocity').value);
    const msg = buildSetVelocity(velocity);
    sendMessage(msg);
}

function setVelocityAndSteps() {
    const velocity = parseInt(document.getElementById('velocityStepsVelocity').value);
    const steps = parseInt(document.getElementById('velocityStepsSteps').value);
    const mode = parseInt(document.getElementById('velocityStepsMode').value);
    const msg = buildSetVelocityAndSteps(velocity, steps, mode);
    sendMessage(msg);
}

function startPath() {
    const pathId = parseInt(document.getElementById('pathId').value);
    const msg = buildStartPath(pathId);
    sendMessage(msg);
}