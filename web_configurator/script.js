let port;
let buffer = new Uint8Array();
let udpEventSource = null;

// Initialize status and start retro effects
document.addEventListener('DOMContentLoaded', function() {
    const statusElement = document.getElementById('status');
    const indicatorElement = document.getElementById('connectionIndicator');
    statusElement.className = 'disconnected';
    indicatorElement.className = 'connection-indicator disconnected';
    
    // Start timestamp update
    updateTimestamp();
    setInterval(updateTimestamp, 1000);
    
    // Add terminal boot effect
    setTimeout(() => {
        document.body.classList.add('booted');
    }, 800);

    // Show/hide UDP target inputs based on connection type
    const connSel = document.getElementById('connectionType');
    const udpIp = document.getElementById('udpTargetIp');
    const udpPort = document.getElementById('udpTargetPort');
    const mainIp = document.getElementById('ethernetAddress');
    const mainPort = document.getElementById('ethernetPort');

    function updateUdpFieldsVisibility() {
        if (!connSel) return;
        const visible = connSel.value === 'udp';
        if (udpIp) udpIp.style.display = visible ? 'inline-block' : 'none';
        if (udpPort) udpPort.style.display = visible ? 'inline-block' : 'none';
    }

    if (connSel) {
        connSel.addEventListener('change', updateUdpFieldsVisibility);
        updateUdpFieldsVisibility();
    }

    // Keep the main ethernet fields and the shortcut fields in sync
    if (udpIp && mainIp) {
        // populate shortcut from main on load
        if (mainIp.value && !udpIp.value) udpIp.value = mainIp.value;
        udpIp.addEventListener('input', () => { mainIp.value = udpIp.value; });
        mainIp.addEventListener('input', () => { udpIp.value = mainIp.value; });
    }
    if (udpPort && mainPort) {
        if (mainPort.value && !udpPort.value) udpPort.value = mainPort.value;
        udpPort.addEventListener('input', () => { mainPort.value = udpPort.value; });
        mainPort.addEventListener('input', () => { udpPort.value = mainPort.value; });
    }
});

// Update timestamp display
function updateTimestamp() {
    const now = new Date();
    const timestamp = now.toLocaleString('en-US', {
        year: 'numeric',
        month: '2-digit',
        day: '2-digit',
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit',
        hour12: false
    }).replace(/[/,]/g, '-').replace(' ', ' ');
    
    const timestampElement = document.getElementById('timestamp');
    if (timestampElement) {
        timestampElement.textContent = timestamp;
    }
}

document.getElementById('connect').addEventListener('click', function() {
    console.log('Connect button clicked');
    connect();
});

document.getElementById('disconnect').addEventListener('click', function() {
    console.log('Disconnect button clicked');
    disconnect();
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
document.getElementById('pollAllSettings').addEventListener('click', pollAllSettings);
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
    const connectionType = document.getElementById('connectionType') ? document.getElementById('connectionType').value : 'serial';

    statusElement.textContent = 'CONNECTING';
    statusElement.className = 'connecting';
    indicatorElement.className = 'connection-indicator connecting';

    if (connectionType === 'serial') {
        // Check if Web Serial API is supported
        if (!('serial' in navigator)) {
            alert('Web Serial API not supported. Please use Chrome/Edge 89+ and ensure the page is served over HTTPS.');
            statusElement.textContent = 'OFFLINE';
            statusElement.className = 'disconnected';
            indicatorElement.className = 'connection-indicator disconnected';
            return;
        }

        try {
            port = await navigator.serial.requestPort();
            await port.open({ baudRate: 115200 });
            statusElement.textContent = 'ONLINE';
            statusElement.className = 'connected';
            indicatorElement.className = 'connection-indicator connected';
            const reader = port.readable.getReader();
            readLoop(reader);
        } catch (error) {
            console.error('Error connecting:', error);
            statusElement.textContent = 'CONNECTION FAILED';
            statusElement.className = 'disconnected';
            indicatorElement.className = 'connection-indicator disconnected';
        }
    } else if (connectionType === 'udp') {
        // Open SSE to receive incoming UDP packets from the server
        try {
            if (udpEventSource) udpEventSource.close();
            udpEventSource = new EventSource('/udp/stream');
            udpEventSource.onmessage = function(e) {
                try {
                    const obj = JSON.parse(e.data);
                    const b64 = obj.payload;
                    const bytes = base64ToUint8Array(b64);
                    processData(bytes);
                } catch (err) {
                    console.error('Error parsing UDP SSE message:', err, e.data);
                }
            };
            udpEventSource.onerror = function(err) {
                console.error('UDP EventSource error', err);
            };

            statusElement.textContent = 'ONLINE';
            statusElement.className = 'connected';
            indicatorElement.className = 'connection-indicator connected';
        } catch (error) {
            console.error('Error connecting (UDP):', error);
            statusElement.textContent = 'CONNECTION FAILED';
            statusElement.className = 'disconnected';
            indicatorElement.className = 'connection-indicator disconnected';
        }
    }
}

async function disconnect() {
    const statusElement = document.getElementById('status');
    const indicatorElement = document.getElementById('connectionIndicator');
    const connectionType = document.getElementById('connectionType') ? document.getElementById('connectionType').value : 'serial';

    if (connectionType === 'serial') {
        if (!port) {
            console.log('No active connection to disconnect');
            return;
        }

        statusElement.textContent = 'DISCONNECTING';
        statusElement.className = 'connecting';
        indicatorElement.className = 'connection-indicator connecting';

        try {
            // Close the port if it's open
            if (port.readable && !port.readable.locked) {
                const reader = port.readable.getReader();
                await reader.cancel();
                reader.releaseLock();
            }

            if (port.writable && !port.writable.locked) {
                const writer = port.writable.getWriter();
                await writer.close();
            }

            await port.close();
            port = null;
            buffer = new Uint8Array();

            statusElement.textContent = 'OFFLINE';
            statusElement.className = 'disconnected';
            indicatorElement.className = 'connection-indicator disconnected';

            console.log('[SERIAL] Connection closed successfully');
        } catch (error) {
            console.error('Error disconnecting:', error);
            statusElement.textContent = 'OFFLINE';
            statusElement.className = 'disconnected';
            indicatorElement.className = 'connection-indicator disconnected';
            port = null;
            buffer = new Uint8Array();
        }
    } else if (connectionType === 'udp') {
        if (udpEventSource) {
            udpEventSource.close();
            udpEventSource = null;
        }
        buffer = new Uint8Array();
        statusElement.textContent = 'OFFLINE';
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
        statusElement.textContent = 'CONNECTION LOST';
        statusElement.className = 'disconnected';
        indicatorElement.className = 'connection-indicator disconnected';
    }
}

function processData(data) {
    // Log all incoming raw bytes
    const hexBytes = Array.from(data).map(b => b.toString(16).padStart(2, '0').toUpperCase()).join(' ');
    console.log(`[SERIAL RX] Received ${data.length} bytes: ${hexBytes}`);
    console.log(`[SERIAL RX] Raw bytes (decimal): [${Array.from(data).join(', ')}]`);
    
    buffer = new Uint8Array([...buffer, ...data]);
    console.log(`[SERIAL] Buffer now has ${buffer.length} bytes total`);
    
    while (buffer.length >= 6) {
        try {
            const view = new DataView(buffer.buffer.slice(buffer.byteOffset, buffer.byteOffset + buffer.length));
            const message_type = view.getUint16(0, true);
            const body_size = view.getUint16(2, true);
            const total_size = 4 + body_size + 2;
            
            console.log(`[SERIAL] Found potential message: Type=0x${message_type.toString(16).padStart(4, '0').toUpperCase()}, BodySize=${body_size}, TotalSize=${total_size}`);
            
            if (buffer.length < total_size) {
                console.log(`[SERIAL] Need more data: have ${buffer.length}, need ${total_size}`);
                break;
            }
            
            const message = buffer.slice(0, total_size);
            buffer = buffer.slice(total_size);
            
            // Log the complete message being processed
            const msgHex = Array.from(message).map(b => b.toString(16).padStart(2, '0').toUpperCase()).join(' ');
            console.log(`[SERIAL] Processing complete message (${total_size} bytes): ${msgHex}`);
            
            // Verify checksum before processing
            if (verifyChecksum(message)) {
                console.log('[SERIAL] ✓ Checksum valid');
                handleMessage(message);
            } else {
                console.error('[SERIAL] ✗ Checksum invalid - message corrupted:', msgHex);
                // Continue processing in case there are other valid messages in buffer
            }
            
        } catch (error) {
            console.error(`[SERIAL] Error processing data: ${error.message}`);
            // If we can't parse header, try to recover by dropping one byte
            if (buffer.length > 0) {
                console.warn(`[SERIAL] Dropping one byte to try to resync: 0x${buffer[0].toString(16).padStart(2, '0').toUpperCase()}`);
                buffer = buffer.slice(1);
            }
        }
    }
    
    if (buffer.length > 0) {
        const bufferHex = Array.from(buffer).map(b => b.toString(16).padStart(2, '0').toUpperCase()).join(' ');
        console.log(`[SERIAL] ${buffer.length} bytes remaining in buffer: ${bufferHex}`);
    }
}

function handleMessage(msg) {
    const view = new DataView(msg.buffer);
    const message_type = view.getUint16(0, true);
    const body_size = view.getUint16(2, true);
    
    // Find message type name for logging
    const messageTypeName = Object.keys(MESSAGE_TYPES).find(key => MESSAGE_TYPES[key] === message_type) || 'UNKNOWN';
    console.log(`[PROTOCOL] Handling message: ${messageTypeName} (0x${message_type.toString(16).padStart(4, '0').toUpperCase()})`);
    
    try {
        switch (message_type) {
            case MESSAGE_TYPES.AckId:
                const ack = parseAck(msg);
                const ackTypeName = Object.keys(MESSAGE_TYPES).find(key => MESSAGE_TYPES[key] === ack.ackType) || 'UNKNOWN';
                console.log(`[PROTOCOL] ACK received: ${ackTypeName} (0x${ack.ackType.toString(16).padStart(4, '0').toUpperCase()}) Status: ${ack.status}`);
                console.log('ACK for', ack.ackType.toString(16), 'status', ack.status);
                break;
        case MESSAGE_TYPES.GetVersionId:
            const ver = parseGetVersion(msg);
            document.getElementById('version').textContent = `v${ver.version}`;
            break;
        case MESSAGE_TYPES.GetI2CAddressId:
            const i2c = parseGetI2CAddress(msg);
            // Update the input field with current value
            document.getElementById('i2cAddress').value = i2c.address;
            console.log(`[PROTOCOL] Current I2C Address: 0x${i2c.address.toString(16).toUpperCase().padStart(2, '0')}`);
            break;
        case MESSAGE_TYPES.GetEthernetAddressId:
            const eth = parseGetEthernetAddress(msg);
            // Update the input field with current value
            document.getElementById('ethernetAddress').value = ipToString(eth.ipAddress);
            console.log(`[PROTOCOL] Current Ethernet Address: ${ipToString(eth.ipAddress)}`);
            break;
        case MESSAGE_TYPES.GetEthernetPortId:
            const port = parseGetEthernetPort(msg);
            // Update the input field with current value
            document.getElementById('ethernetPort').value = port.port;
            console.log(`[PROTOCOL] Current Ethernet Port: ${port.port}`);
            break;
        case MESSAGE_TYPES.GetMacAddressId:
            const mac = parseGetMacAddress(msg);
            document.getElementById('currentMacAddress').textContent = mac.mac.map(b => b.toString(16).padStart(2, '0').toUpperCase()).join(':');
            break;
        case MESSAGE_TYPES.GetLedColorId:
            const color = parseGetLedColor(msg);
            // Update the input fields with current values
            document.getElementById('ledR').value = color.r;
            document.getElementById('ledG').value = color.g;
            document.getElementById('ledB').value = color.b;
            console.log(`[PROTOCOL] Current LED Color: RGB(${color.r},${color.g},${color.b})`);
            break;
        case MESSAGE_TYPES.GetHomeDirectionId:
            const dir = parseGetHomeDirection(msg);
            // Update the select field with current value
            document.getElementById('homeDirection').value = dir.direction;
            console.log(`[PROTOCOL] Current Home Direction: ${dir.direction === 0 ? 'CLOCKWISE' : 'COUNTER-CLOCKWISE'}`);
            break;
        case MESSAGE_TYPES.GetHomeThresholdId:
            const thresh = parseGetHomeThreshold(msg);
            // Update the input field with current value
            document.getElementById('homeThreshold').value = thresh.threshold;
            console.log(`[PROTOCOL] Current Home Threshold: ${thresh.threshold}`);
            break;
        case MESSAGE_TYPES.GetHomeSpeedId:
            const hspeed = parseGetHomeSpeed(msg);
            // Update the input field with current value
            document.getElementById('homeSpeed').value = hspeed.speed;
            console.log(`[PROTOCOL] Current Home Speed: ${hspeed.speed}`);
            break;
        case MESSAGE_TYPES.GetHomedStateId:
            const homed = parseGetHomedState(msg);
            document.getElementById('currentHomedState').textContent = homed.homed ? 'HOMED' : 'NOT_HOMED';
            break;
        case MESSAGE_TYPES.GetMotorStateId:
            const mstate = parseGetMotorState(msg);
            const stateNames = ['OFF', 'POSITION', 'VELOCITY', 'VELOCITY_STEP', 'IDLE_ON', 'HOME'];
            // Update the select field with current value
            document.getElementById('motorState').value = mstate.motorState;
            console.log(`[PROTOCOL] Current Motor State: ${stateNames[mstate.motorState] || 'UNKNOWN'} (${mstate.motorState})`);
            break;
        case MESSAGE_TYPES.GetMotorBrakeId:
            const brake = parseGetMotorBrake(msg);
            const brakeNames = ['NORMAL', 'FREEWHEELING', 'STRONG_BRAKING', 'BRAKING'];
            // Update the select field with current value
            document.getElementById('motorBrake').value = brake.brakeMode;
            console.log(`[PROTOCOL] Current Motor Brake: ${brakeNames[brake.brakeMode] || 'UNKNOWN'} (${brake.brakeMode})`);
            break;
        case MESSAGE_TYPES.GetMaxSpeedId:
            const maxspd = parseGetMaxSpeed(msg);
            // Update the input field with current value
            document.getElementById('maxSpeed').value = maxspd.maxSpeed;
            console.log(`[PROTOCOL] Current Max Speed: ${maxspd.maxSpeed}`);
            break;
        case MESSAGE_TYPES.GetAccelerationId:
            const accel = parseGetAcceleration(msg);
            // Update the input field with current value
            document.getElementById('acceleration').value = accel.acceleration;
            console.log(`[PROTOCOL] Current Acceleration: ${accel.acceleration}`);
            break;
        case MESSAGE_TYPES.GetCurrentPositionId:
            const cpos = parseGetCurrentPosition(msg);
            // Update the input field with current value
            document.getElementById('currentPosition').value = cpos.position.toFixed(2);
            console.log(`[PROTOCOL] Current Position: ${cpos.position.toFixed(2)}°`);
            break;
        case MESSAGE_TYPES.GetTargetPositionId:
            const tpos = parseGetTargetPosition(msg);
            // Update the input field with current value
            document.getElementById('targetPosition').value = tpos.position.toFixed(2);
            console.log(`[PROTOCOL] Current Target Position: ${tpos.position.toFixed(2)}°`);
            break;
        case MESSAGE_TYPES.GetVelocityId:
            const vel = parseGetVelocity(msg);
            // Update the input field with current value
            document.getElementById('velocity').value = vel.velocity.toFixed(2);
            console.log(`[PROTOCOL] Current Velocity: ${vel.velocity.toFixed(2)}°/s`);
            break;
        default:
            console.warn(`[PROTOCOL] Unknown message type: 0x${message_type.toString(16).padStart(4, '0').toUpperCase()}`);
            const unknownHex = Array.from(msg).map(b => b.toString(16).padStart(2, '0').toUpperCase()).join(' ');
            console.log(`[PROTOCOL] Unknown message bytes: ${unknownHex}`);
            console.log('Unknown message type:', message_type.toString(16));
    }
    } catch (error) {
        console.error(`[PROTOCOL] Error parsing message: ${error.message}`);
        const errorHex = Array.from(msg).map(b => b.toString(16).padStart(2, '0').toUpperCase()).join(' ');
        console.error(`[PROTOCOL] Failed message bytes: ${errorHex}`);
        console.error('Message parsing error:', error);
    }
}

async function sendMessage(msg) {
    const connectionType = document.getElementById('connectionType') ? document.getElementById('connectionType').value : 'serial';

    if (connectionType === 'udp') {
        // Send payload to server which will forward over UDP
        // Prefer the nearby udpTarget inputs if present
        const ipField = document.getElementById('udpTargetIp') || document.getElementById('ethernetAddress');
        const portField = document.getElementById('udpTargetPort') || document.getElementById('ethernetPort');
        const ip = ipField ? ipField.value : '';
        const portVal = portField ? parseInt(portField.value) || 0 : 0;
        if (!ip || !portVal) {
            console.error('[UDP TX] Missing target IP or port');
            alert('Please enter target IP and port for UDP connection');
            return;
        }

        const payloadB64 = uint8ArrayToBase64(msg);
        try {
            const resp = await fetch('/udp/send', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ ip: ip, port: portVal, payload: payloadB64 })
            });
            if (!resp.ok) console.error('[UDP TX] Send failed', resp.statusText);
            else console.log('[UDP TX] Sent', msg.length, 'bytes to', ip + ':' + portVal);
        } catch (err) {
            console.error('[UDP TX] Error sending:', err);
        }

        // Log message locally as UDP TX
        const hexBytes = Array.from(msg).map(b => b.toString(16).padStart(2, '0').toUpperCase()).join(' ');
        console.log(`[UDP TX] Sending ${msg.length} bytes: ${hexBytes}`);
        return;
    }
    
    // Determine message type for logging
    if (msg.length >= 2) {
        const view = new DataView(msg.buffer, msg.byteOffset);
        const message_type = view.getUint16(0, true);
        const messageTypeName = Object.keys(MESSAGE_TYPES).find(key => MESSAGE_TYPES[key] === message_type) || 'UNKNOWN';
        const hexBytes = Array.from(msg).map(b => b.toString(16).padStart(2, '0').toUpperCase()).join(' ');
        console.log(`[SERIAL TX] Sending message: ${messageTypeName} (0x${message_type.toString(16).padStart(4, '0').toUpperCase()}) - ${msg.length} bytes`);
        console.log(`[SERIAL TX] Bytes: ${hexBytes}`);
    } else {
        const hexBytes = Array.from(msg).map(b => b.toString(16).padStart(2, '0').toUpperCase()).join(' ');
        console.log(`[SERIAL TX] Sending ${msg.length} bytes: ${hexBytes}`);
    }
    
    try {
        const writer = port.writable.getWriter();
        await writer.write(msg);
        writer.releaseLock();
        console.log('[SERIAL TX] Message sent successfully');
    } catch (error) {
        console.error(`[SERIAL TX] Send error: ${error.message}`);
        console.error('Send error:', error);
    }
}

// Helpers for base64 <-> Uint8Array
function uint8ArrayToBase64(u8) {
    let CHUNK_SIZE = 0x8000;
    let index = 0;
    let length = u8.length;
    let result = '';
    while (index < length) {
        let slice = u8.subarray(index, Math.min(index + CHUNK_SIZE, length));
        result += String.fromCharCode.apply(null, slice);
        index += CHUNK_SIZE;
    }
    return btoa(result);
}

function base64ToUint8Array(b64) {
    const binary_string = atob(b64);
    const len = binary_string.length;
    const bytes = new Uint8Array(len);
    for (let i = 0; i < len; i++) {
        bytes[i] = binary_string.charCodeAt(i);
    }
    return bytes;
}

// Utility functions
function ipToString(ip) {
    // IP is stored as little-endian 32-bit integer, so bytes are reversed
    return [
        ip & 0xFF,
        (ip >>> 8) & 0xFF,
        (ip >>> 16) & 0xFF,
        (ip >>> 24) & 0xFF
    ].join('.');
}

function stringToIp(ipString) {
    const parts = ipString.split('.').map(Number);
    // Store as little-endian: first octet in least significant byte
    return parts[0] | (parts[1] << 8) | (parts[2] << 16) | (parts[3] << 24);
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

async function pollAllSettings() {
    console.log('[POLL] Starting poll of all settings...');
    
    const messages = [
        // Basic Configuration
        buildGetVersion(),
        buildGetI2CAddress(),
        buildGetEthernetAddress(),
        buildGetEthernetPort(),
        buildGetMacAddress(),
        
        // LED Control
        buildGetLedColor(),
        
        // Home Configuration
        buildGetHomeDirection(),
        buildGetHomeThreshold(),
        buildGetHomeSpeed(),
        buildGetHomedState(),
        
        // Motor Control
        buildGetMotorState(),
        buildGetMotorBrake(),
        buildGetMaxSpeed(),
        buildGetAcceleration(),
        buildGetCurrentPosition(),
        buildGetTargetPosition(),
        buildGetVelocity()
    ];
    
    // Send messages with delays to avoid WritableStream locking issues
    for (let i = 0; i < messages.length; i++) {
        await sendMessage(messages[i]);
        // Add 50ms delay between messages to prevent stream locking
        await new Promise(resolve => setTimeout(resolve, 50));
    }
    
    console.log('[POLL] All setting queries sent');
}