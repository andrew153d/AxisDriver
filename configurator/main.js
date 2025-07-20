// Set Home Assistant parameters event
const setHaBtn = document.getElementById('setHaBtn');
if (setHaBtn) {
    setHaBtn.addEventListener('click', async () => {
        if (!writer || !port) {
            printToTerminal('Not connected to device.\n');
            return;
        }
        // Collect values from text boxes
        const ha_enable = parseInt(document.getElementById('ha_enable').value) || 0;
        const ha_mode = parseInt(document.getElementById('ha_mode').value) || 0;
        // IP address as 4 bytes
        const ha_ip_str = document.getElementById('ha_ip_address').value;
        const ha_ip_parts = ha_ip_str.split('.').map(x => parseInt(x) || 0);
        const ha_ip_address = new Uint8Array(4);
        for (let i = 0; i < 4; ++i) ha_ip_address[i] = ha_ip_parts[i] || 0;
        const ha_vel_switch_on = parseFloat(document.getElementById('ha_vel_switch_on').value) || 0;
        const ha_vel_switch_off = parseFloat(document.getElementById('ha_vel_switch_off').value) || 0;
        const ha_pos_switch_on = parseFloat(document.getElementById('ha_pos_switch_on').value) || 0;
        const ha_pos_switch_off = parseFloat(document.getElementById('ha_pos_switch_off').value) || 0;
        const ha_vel_slider_min = parseFloat(document.getElementById('ha_vel_slider_min').value) || 0;
        const ha_vel_slider_max = parseFloat(document.getElementById('ha_vel_slider_max').value) || 0;
        const ha_pos_slider_min = parseFloat(document.getElementById('ha_pos_slider_min').value) || 0;
        const ha_pos_slider_max = parseFloat(document.getElementById('ha_pos_slider_max').value);
        console.log('HA Settings:', {
            ha_enable,
            ha_mode,
            ha_ip_address: Array.from(ha_ip_address),
            ha_vel_switch_on,
            ha_vel_switch_off,
            ha_pos_switch_on,
            ha_pos_switch_off,
            ha_vel_slider_min,
            ha_vel_slider_max,
            ha_pos_slider_min,
            ha_pos_slider_max
        });
        // Strings as 32-byte arrays
        function strToBytes(str) {
            const encoder = new TextEncoder();
            const bytes = encoder.encode(str);
            const arr = new Uint8Array(32);
            arr.set(bytes.slice(0, 32));
            return arr;
        }
        const ha_mqtt_user = strToBytes(document.getElementById('ha_mqtt_user').value);
        const ha_mqtt_password = strToBytes(document.getElementById('ha_mqtt_password').value);
        const ha_mqtt_name = strToBytes(document.getElementById('ha_mqtt_name').value);
        const ha_mqtt_icon = strToBytes(document.getElementById('ha_mqtt_icon').value);

        // Send all set messages
        const setMessages = [
            { builder: buildHAEnableMessage, id: MessageTypes.SetHAEnableId, value: ha_enable },
            { builder: buildHAModeMessage, id: MessageTypes.SetHAModeId, value: ha_mode },
            { builder: buildHAIpAddressMessage, id: MessageTypes.SetHAIpAddressId, value: ha_ip_address },
            { builder: buildHAVelocitySwitchOnSpeedMessage, id: MessageTypes.SetHAVelocitySwitchOnSpeedId, value: ha_vel_switch_on },
            { builder: buildHAVelocitySwitchOffSpeedMessage, id: MessageTypes.SetHAVelocitySwitchOffSpeedId, value: ha_vel_switch_off },
            { builder: buildHAPositionSwitchOnPositionMessage, id: MessageTypes.SetHAPositionSwitchOnPositionId, value: ha_pos_switch_on },
            { builder: buildHAPositionSwitchOffPositionMessage, id: MessageTypes.SetHAPositionSwitchOffPositionId, value: ha_pos_switch_off },
            { builder: buildHAVelocitySliderMinMessage, id: MessageTypes.SetHAVelocitySliderMinId, value: ha_vel_slider_min },
            { builder: buildHAVelocitySliderMaxMessage, id: MessageTypes.SetHAVelocitySliderMaxId, value: ha_vel_slider_max },
            { builder: buildHAPositionSliderMinMessage, id: MessageTypes.SetHAPositionSliderMinId, value: ha_pos_slider_min },
            { builder: buildHAPositionSliderMaxMessage, id: MessageTypes.SetHAPositionSliderMaxId, value: ha_pos_slider_max },
            { builder: buildHAMqttUserMessage, id: MessageTypes.SetHAMqttUserId, value: ha_mqtt_user },
            { builder: buildHAMqttPasswordMessage, id: MessageTypes.SetHAMqttPasswordId, value: ha_mqtt_password },
            { builder: buildHAMqttNameMessage, id: MessageTypes.SetHAMqttNameId, value: ha_mqtt_name },
            { builder: buildHAMqttIconMessage, id: MessageTypes.SetHAMqttIconId, value: ha_mqtt_icon },
        ];
        for (const msg of setMessages) {
            let data = msg.builder(msg.id, msg.value);
            await writer.write(data);
            await new Promise(resolve => setTimeout(resolve, 100));
            readLoop(); // Process any incoming messages
        }
        printToTerminal('Set messages sent.\n');
    });
}


// import { MessageTypes, buildEthernetPortMessage, buildEthernetAddressMessage, parseU32Message } from './AxisMessages.js';


let port = null;
let reader = null;
let writer = null;
// Central read buffer and resolver queue for reliable serial reads
let readBuffer = [];
let readResolvers = [];

// Create UI for settings


const connectBtn = document.getElementById('connectBtn');
const statusDiv = document.getElementById('status');
const terminal = document.getElementById('terminal');
const settingsDiv = document.getElementById('settings');
const ethPortSpan = document.getElementById('eth_port');
const ethIpSpan = document.getElementById('eth_ip');
const refreshBtn = document.getElementById('refreshBtn');

function printToTerminal(text) {
    terminal.value += text;
    terminal.scrollTop = terminal.scrollHeight;
}

/**
 * Calculates a 16-bit checksum of a Uint8Array buffer.
 * @param {Uint8Array} buffer - The buffer to checksum.
 * @param {number} length - Number of bytes to include.
 * @returns {number} The 16-bit checksum.
 */
function calculateChecksum(buffer, length) {
    if (!(buffer instanceof Uint8Array)) {
        throw new TypeError('buffer must be a Uint8Array');
    }
    if (typeof length !== 'number') {
        throw new TypeError('length must be a number');
    }
    let sum = 0;
    for (let i = 0; i < length; i++) {
        sum = (sum + buffer[i]) & 0xFFFF;
        //console.log(`Adding byte ${i}: ${buffer[i].toString(16).padStart(2, '0')}, sum: ${sum.toString(16).padStart(4, '0')}`);
    }
    return sum;
}

connectBtn.addEventListener('click', async () => {
    try {
        port = await navigator.serial.requestPort();
        await port.open({ baudRate: 115200 });
        statusDiv.textContent = 'Connected';
        reader = port.readable.getReader();
        writer = port.writable.getWriter();
        printToTerminal('[Connected to device]\n');
        settingsDiv.style.display = '';
        // Start the central read loop
        readLoop();
        await readSettings();
    } catch (e) {
        statusDiv.textContent = 'Connection failed: ' + e;
        printToTerminal('[Connection failed]\n');
    }
});

refreshBtn.addEventListener('click', async () => {
    await readSettings();
});

async function readSettings() {
    // Only send requests, do not wait for responses
    const getterMessages = [
        {builder:  buildGetVers}
        { builder: buildHAEnableMessage, id: MessageTypes.GetHAEnableId },
        { builder: buildHAModeMessage, id: MessageTypes.GetHAModeId },
        { builder: buildHAIpAddressMessage, id: MessageTypes.GetHAIpAddressId },
        { builder: buildHAVelocitySwitchOnSpeedMessage, id: MessageTypes.GetHAVelocitySwitchOnSpeedId },
        { builder: buildHAVelocitySwitchOffSpeedMessage, id: MessageTypes.GetHAVelocitySwitchOffSpeedId },
        { builder: buildHAPositionSwitchOnPositionMessage, id: MessageTypes.GetHAPositionSwitchOnPositionId },
        { builder: buildHAPositionSwitchOffPositionMessage, id: MessageTypes.GetHAPositionSwitchOffPositionId },
        { builder: buildHAVelocitySliderMinMessage, id: MessageTypes.GetHAVelocitySliderMinId },
        { builder: buildHAVelocitySliderMaxMessage, id: MessageTypes.GetHAVelocitySliderMaxId },
        { builder: buildHAPositionSliderMinMessage, id: MessageTypes.GetHAPositionSliderMinId },
        { builder: buildHAPositionSliderMaxMessage, id: MessageTypes.GetHAPositionSliderMaxId },
        { builder: buildHAMqttUserMessage, id: MessageTypes.GetHAMqttUserId },
        { builder: buildHAMqttPasswordMessage, id: MessageTypes.GetHAMqttPasswordId },
        { builder: buildHAMqttNameMessage, id: MessageTypes.GetHAMqttNameId },
        { builder: buildHAMqttIconMessage, id: MessageTypes.GetHAMqttIconId }
    ];

    for (const { builder, id } of getterMessages) {
        const msg = builder(id, 0);
        await writer.write(msg);
        readLoop();
        await new Promise(resolve => setTimeout(resolve, 200)); // 100ms delay between messages
        readLoop();
    }
}

function u32ToIp(ip) {
    return [
        ip & 0xFF,
        (ip >> 8) & 0xFF,
        (ip >> 16) & 0xFF,
        (ip >> 24) & 0xFF,
    ].join('.');
}

// Central read loop: buffers all incoming data and resolves pending binary reads
async function readLoop() {
    const decoder = new TextDecoder();
    try {
        while (port && port.readable) {
            const { value, done } = await reader.read();
            if (done) break;
            if (value) {
                // Buffer the data for binary consumers
                readBuffer.push(...value);

                if (value.some(byte => byte < 32 || byte > 126)) {
                    // Binary message, do not print
                } else {
                    printToTerminal(decoder.decode(value));
                }

                // Process all complete messages in the buffer
                processReadBuffer();
            }
        }
    } catch (e) {
        printToTerminal('[Read error: ' + e + ']\n');
    }
}

// Helper to process all complete messages in the readBuffer
function processReadBuffer() {
    while (readBuffer.length >= 4) {
        // Search for sync bytes
        if (
            readBuffer[0] !== (SYNC_BYTES >> 0 & 0xFF) ||
            readBuffer[1] !== (SYNC_BYTES >> 8 & 0xFF) ||
            readBuffer[2] !== (SYNC_BYTES >> 16 & 0xFF) ||
            readBuffer[3] !== (SYNC_BYTES >> 24 & 0xFF)
        ) {
            // Remove first byte and continue searching
            readBuffer.shift();
            continue;
        }
        // We have sync bytes, check if we have enough for a header
        if (readBuffer.length < HEADER_SIZE) {
            // Wait for more data
            break;
        }
        let header = parseHeader(Uint8Array.from(readBuffer.slice(0, HEADER_SIZE)).buffer);
        const totalMsgLen = HEADER_SIZE + header.body_size + FOOTER_SIZE;
        if (readBuffer.length < totalMsgLen) {
            // Wait for more data
            break;
        }
        // We have a full message
        const footer = parseFooter(Uint8Array.from(readBuffer.slice(HEADER_SIZE + header.body_size, HEADER_SIZE + header.body_size + FOOTER_SIZE)).buffer);
        const calculatedChecksum = calculateChecksum(Uint8Array.from(readBuffer.slice(0, HEADER_SIZE + header.body_size)), HEADER_SIZE + header.body_size);
        if (footer.checksum === calculatedChecksum) {
            handleMessage(Uint8Array.from(readBuffer.slice(0, totalMsgLen)).buffer);
        } else {
            console.warn('Invalid checksum for message:', footer.checksum, '!=', calculatedChecksum);
        }
        // Remove processed message
        readBuffer.splice(0, totalMsgLen);
    }
}

async function handleMessage(message) {
    const header = parseHeader(message);
    console.log('Received message:', header);
    if (header.message_type === MessageTypes.GetEthernetPortId) {
        const portId = parseEthernetPortMessage(message);
        ethPortSpan.textContent = portId.value.toString();
        printToTerminal(`Ethernet Port ID: ${portId.value}\n`);
    } else if (header.message_type === MessageTypes.GetEthernetAddressId) {
        const ip = parseEthernetAddressMessage(message).value;
        ethIpSpan.textContent = u32ToIp(ip);
        printToTerminal(`Ethernet IP Address: ${u32ToIp(ip)}\n`);
    } else if (header.message_type === MessageTypes.GetHAEnableId) {
        const enabled = parseHAEnableMessage(message).value;
        document.getElementById('ha_enable').value = enabled;
        printToTerminal(`HA Enable: ${enabled}\n`);
    } else if (header.message_type === MessageTypes.GetHAModeId) {
        const mode = parseHAModeMessage(message).value;
        document.getElementById('ha_mode').value = mode;
        printToTerminal(`HA Mode: ${mode}\n`);
    } else if (header.message_type === MessageTypes.GetHAIpAddressId) {
        const ip = parseHAIpAddressMessage(message).ha_ip_address;
        document.getElementById('ha_ip_address').value = Array.from(ip).join('.');
        printToTerminal(`HA IP Address: ${Array.from(ip).join('.')}\n`);
    } else if (header.message_type === MessageTypes.GetHAVelocitySwitchOnSpeedId) {
        const val = parseHAVelocitySwitchOnSpeedMessage(message).value;
        document.getElementById('ha_vel_switch_on').value = val;
        printToTerminal(`HA Velocity Switch On Speed: ${val}\n`);
    } else if (header.message_type === MessageTypes.GetHAVelocitySwitchOffSpeedId) {
        const val = parseHAVelocitySwitchOffSpeedMessage(message).value;
        document.getElementById('ha_vel_switch_off').value = val;
        printToTerminal(`HA Velocity Switch Off Speed: ${val}\n`);
    } else if (header.message_type === MessageTypes.GetHAPositionSwitchOnPositionId) {
        const val = parseHAPositionSwitchOnPositionMessage(message).value;
        document.getElementById('ha_pos_switch_on').value = val;
        printToTerminal(`HA Position Switch On Position: ${val}\n`);
    } else if (header.message_type === MessageTypes.GetHAPositionSwitchOffPositionId) {
        const val = parseHAPositionSwitchOffPositionMessage(message).value;
        document.getElementById('ha_pos_switch_off').value = val;
        printToTerminal(`HA Position Switch Off Position: ${val}\n`);
    } else if (header.message_type === MessageTypes.GetHAVelocitySliderMinId) {
        const val = parseHAVelocitySliderMinMessage(message).value;
        document.getElementById('ha_vel_slider_min').value = val;
        printToTerminal(`HA Velocity Slider Min: ${val}\n`);
    } else if (header.message_type === MessageTypes.GetHAVelocitySliderMaxId) {
        const val = parseHAVelocitySliderMaxMessage(message).value;
        document.getElementById('ha_vel_slider_max').value = val;
        printToTerminal(`HA Velocity Slider Max: ${val}\n`);
    } else if (header.message_type === MessageTypes.GetHAPositionSliderMinId) {
        const val = parseHAPositionSliderMinMessage(message).value;
        document.getElementById('ha_pos_slider_min').value = val;
        printToTerminal(`HA Position Slider Min: ${val}\n`);
    } else if (header.message_type === MessageTypes.GetHAPositionSliderMaxId) {
        const val = parseHAPositionSliderMaxMessage(message).value;
        document.getElementById('ha_pos_slider_max').value = val;
        printToTerminal(`HA Position Slider Max: ${val}\n`);
    } else if (header.message_type === MessageTypes.GetHAMqttUserId) {
        const user = parseHAMqttUserMessage(message).value;
        const userStr = new TextDecoder().decode(user);
        const zeroIdx = userStr.indexOf('\0');
        document.getElementById('ha_mqtt_user').value = zeroIdx !== -1 ? userStr.slice(0, zeroIdx) : userStr;
        printToTerminal(`HA MQTT User: ${zeroIdx !== -1 ? userStr.slice(0, zeroIdx) : userStr}\n`);
    } else if (header.message_type === MessageTypes.GetHAMqttPasswordId) {
        const pwd = parseHAMqttPasswordMessage(message).value;
        const pwdStr = new TextDecoder().decode(pwd);
        const zeroIdx = pwdStr.indexOf('\0');
        document.getElementById('ha_mqtt_password').value = zeroIdx !== -1 ? pwdStr.slice(0, zeroIdx) : pwdStr;
        printToTerminal(`HA MQTT Password: ${zeroIdx !== -1 ? pwdStr.slice(0, zeroIdx) : pwdStr}\n`);
    } else if (header.message_type === MessageTypes.GetHAMqttNameId) {
        const name = parseHAMqttNameMessage(message).value;
        const nameStr = new TextDecoder().decode(name);
        const zeroIdx = nameStr.indexOf('\0');
        document.getElementById('ha_mqtt_name').value = zeroIdx !== -1 ? nameStr.slice(0, zeroIdx) : nameStr;
        printToTerminal(`HA MQTT Name: ${zeroIdx !== -1 ? nameStr.slice(0, zeroIdx) : nameStr}\n`);
    } else if (header.message_type === MessageTypes.GetHAMqttIconId) {
        const icon = parseHAMqttIconMessage(message).value;
        const iconStr = new TextDecoder().decode(icon);
        const zeroIdx = iconStr.indexOf('\0');
        document.getElementById('ha_mqtt_icon').value = zeroIdx !== -1 ? iconStr.slice(0, zeroIdx) : iconStr;
        printToTerminal(`HA MQTT Icon: ${zeroIdx !== -1 ? iconStr.slice(0, zeroIdx) : iconStr}\n`);
    } else {
        printToTerminal(`Unknown message type: ${header.message_type}\n`);
        console.log('Raw message bytes:', Array.from(new Uint8Array(message)).map(b => b.toString(16).padStart(2, '0')).join(' '));
    }
}

async function sendInput() {
    if (!writer || !port) return;
    const text = inputLine.value;
    if (text.length === 0) return;
    const encoder = new TextEncoder();
    await writer.write(encoder.encode(text + '\n'));
    inputLine.value = '';
}
