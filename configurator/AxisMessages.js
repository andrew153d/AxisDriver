
// Constants
const SYNC_BYTES = 0xDEADBABE;  // Sync bytes for message start
const HEADER_SIZE = 8;  // Size of the header in bytes
const FOOTER_SIZE = 2;  // Size of the footer in bytes

const MessageTypes = Object.freeze({
    GetVersionId: 0x0,
    SetI2CAddressId: 0x1,
    GetI2CAddressId: 0x2,
    SetEthernetAddressId: 0x3,
    GetEthernetAddressId: 0x4,
    SetEthernetPortId: 0x5,
    GetEthernetPortId: 0x6,
    GetMacAddressId: 0x7,
    SaveConfigurationId: 0x8,
    SetLedStateId: 0x9,
    GetLedStateId: 0xA,
    SetLedColorId: 0xB,
    GetLedColorId: 0xC,
    SetHomeDirectionId: 0xD,
    GetHomeDirectionId: 0xE,
    SetHomeThresholdId: 0xF,
    GetHomeThresholdId: 0x10,
    SetHomeSpeedId: 0x11,
    GetHomeSpeedId: 0x12,
    GetHomedStateId: 0x13,
    HomeId: 0x14,
    SetMotorStateId: 0x15,
    GetMotorStateId: 0x16,
    SetMotorBrakeId: 0x17,
    GetMotorBrakeId: 0x18,
    SetMaxSpeedId: 0x19,
    GetMaxSpeedId: 0x1A,
    SetAccelerationId: 0x1B,
    GetAccelerationId: 0x1C,
    SetCurrentPositionId: 0x1D,
    GetCurrentPositionId: 0x1E,
    SetTargetPositionId: 0x1F,
    GetTargetPositionId: 0x20,
    SetRelativeTargetPositionId: 0x21,
    SetVelocityId: 0x22,
    GetVelocityId: 0x23,
    SetVelocityAndStepsId: 0x24,
    StartPathId: 0x25,
    SetHAEnableId: 0x26,
    GetHAEnableId: 0x27,
    SetHAModeId: 0x28,
    GetHAModeId: 0x29,
    SetHAIpAddressId: 0x2A,
    GetHAIpAddressId: 0x2B,
    SetHAVelocitySwitchOnSpeedId: 0x2C,
    GetHAVelocitySwitchOnSpeedId: 0x2D,
    SetHAVelocitySwitchOffSpeedId: 0x2E,
    GetHAVelocitySwitchOffSpeedId: 0x2F,
    SetHAPositionSwitchOnPositionId: 0x30,
    GetHAPositionSwitchOnPositionId: 0x31,
    SetHAPositionSwitchOffPositionId: 0x32,
    GetHAPositionSwitchOffPositionId: 0x33,
    SetHAVelocitySliderMinId: 0x34,
    GetHAVelocitySliderMinId: 0x35,
    SetHAVelocitySliderMaxId: 0x36,
    GetHAVelocitySliderMaxId: 0x37,
    SetHAPositionSliderMinId: 0x38,
    GetHAPositionSliderMinId: 0x39,
    SetHAPositionSliderMaxId: 0x3A,
    GetHAPositionSliderMaxId: 0x3B,
    SetHAMqttUserId: 0x3C,
    GetHAMqttUserId: 0x3D,
    SetHAMqttPasswordId: 0x3E,
    GetHAMqttPasswordId: 0x3F,
    SetHAMqttNameId: 0x40,
    GetHAMqttNameId: 0x41,
    SetHAMqttIconId: 0x42,
    GetHAMqttIconId: 0x43,
	MaxMessageType: 0x44
});

// LedStates enum
const LedStates = Object.freeze({
    OFF: 0x0,
    FLASH_ERROR: 0x1,
    ERROR: 0x2,
    BOOTUP: 0x3,
    RAINBOW: 0x4,
    SOLID: 0x5,
    MAX_VALUE: 0x6,
});

// MotorStates enum
const MotorStates = Object.freeze({
    OFF: 0x0,
    POSITION: 0x1,
    VELOCITY: 0x2,
    VELOCITY_STEP: 0x3,
    IDLE_ON: 0x4,
    HOME: 0x5,
});

// MotorBrake enum
const MotorBrake = Object.freeze({
    NORMAL: 0x0,
    FREEWHEELING: 0x1,
    STRONG_BRAKING: 0x2,
    BRAKING: 0x3,
});

// HomeDirection enum
const HomeDirection = Object.freeze({
    CLOCKWISE: 0x0,
    COUNTERCLOCKWISE: 0x1,
});

// PositionMode enum
const PositionMode = Object.freeze({
    ABSOLUTE: 0x0,
    RELATIVE: 0x1,
});

// Header message builder
function buildHeader(sync_bytes, message_type, body_size) {
	const totalLength = 8;
	let offset = 0;
	const buffer = new Uint8Array(totalLength);
	const view = new DataView(buffer.buffer);
	view.setUint32(offset, sync_bytes, true);
	offset += 4;
	view.setUint16(offset, message_type, true);
	offset += 2;
	view.setUint16(offset, body_size, true);
	offset += 2;
	return buffer;
}

// Header message parser
function parseHeader(buffer){
	const dv = new DataView(buffer);
	let offset = 0;
	const sync_bytes = dv.getUint32(offset, true);
	offset += 4;
	const message_type = dv.getUint16(offset, true);
	offset += 2;
	const body_size = dv.getUint16(offset, true);
	offset += 2;
	return {
sync_bytes	, message_type	, body_size
	};
}
// Footer message builder
function buildFooter(checksum) {
	const totalLength = 2;
	let offset = 0;
	const buffer = new Uint8Array(totalLength);
	const view = new DataView(buffer.buffer);
	view.setUint16(offset, checksum, true);
	offset += 2;
	return buffer;
}

// Footer message parser
function parseFooter(buffer){
	const dv = new DataView(buffer);
	let offset = 0;
	const checksum = dv.getUint16(offset, true);
	offset += 2;
	return {
checksum
	};
}
// U8Message message builder
function buildU8Message(msgId, value) {
	const totalLength = 11;
	let offset = 0;
	const buffer = new Uint8Array(totalLength);
	const view = new DataView(buffer.buffer);
	const headerBytes = buildHeader(SYNC_BYTES, msgId, totalLength-(HEADER_SIZE+FOOTER_SIZE));
	buffer.set(headerBytes, offset);
	offset += headerBytes.length;
	view.setUint8(offset, value, true);
	offset += 1;
	const footerBytes = buildFooter(0);
	buffer.set(footerBytes, offset);
	return buffer;
}

// U8Message message parser
function parseU8Message(buffer){
	const dv = new DataView(buffer);
	let offset = 0;
	const syncBytes = dv.getUint32(offset, true);
	offset += 4;
	const messageType = dv.getUint16(offset, true);
	offset += 2;
	const bodySize = dv.getUint16(offset, true);
	offset += 2;
	const value = dv.getUint8(offset, true);
	offset += 1;
	const checksum = dv.getUint16(offset, true);
	offset += 2;
	return {
	messageType, bodySize, 
value
	, checksum
	};
}
// S8Message message builder
function buildS8Message(msgId, value) {
	const totalLength = 11;
	let offset = 0;
	const buffer = new Uint8Array(totalLength);
	const view = new DataView(buffer.buffer);
	const headerBytes = buildHeader(SYNC_BYTES, msgId, totalLength-(HEADER_SIZE+FOOTER_SIZE));
	buffer.set(headerBytes, offset);
	offset += headerBytes.length;
	view.setInt8(offset, value, true);
	offset += 1;
	const footerBytes = buildFooter(0);
	buffer.set(footerBytes, offset);
	return buffer;
}

// S8Message message parser
function parseS8Message(buffer){
	const dv = new DataView(buffer);
	let offset = 0;
	const syncBytes = dv.getUint32(offset, true);
	offset += 4;
	const messageType = dv.getUint16(offset, true);
	offset += 2;
	const bodySize = dv.getUint16(offset, true);
	offset += 2;
	const value = dv.getInt8(offset, true);
	offset += 1;
	const checksum = dv.getUint16(offset, true);
	offset += 2;
	return {
	messageType, bodySize, 
value
	, checksum
	};
}
// U32Message message builder
function buildU32Message(msgId, value) {
	const totalLength = 14;
	let offset = 0;
	const buffer = new Uint8Array(totalLength);
	const view = new DataView(buffer.buffer);
	const headerBytes = buildHeader(SYNC_BYTES, msgId, totalLength-(HEADER_SIZE+FOOTER_SIZE));
	buffer.set(headerBytes, offset);
	offset += headerBytes.length;
	view.setUint32(offset, value, true);
	offset += 4;
	const footerBytes = buildFooter(0);
	buffer.set(footerBytes, offset);
	return buffer;
}

// U32Message message parser
function parseU32Message(buffer){
	const dv = new DataView(buffer);
	let offset = 0;
	const syncBytes = dv.getUint32(offset, true);
	offset += 4;
	const messageType = dv.getUint16(offset, true);
	offset += 2;
	const bodySize = dv.getUint16(offset, true);
	offset += 2;
	const value = dv.getUint32(offset, true);
	offset += 4;
	const checksum = dv.getUint16(offset, true);
	offset += 2;
	return {
	messageType, bodySize, 
value
	, checksum
	};
}
// S32Message message builder
function buildS32Message(msgId, value) {
	const totalLength = 14;
	let offset = 0;
	const buffer = new Uint8Array(totalLength);
	const view = new DataView(buffer.buffer);
	const headerBytes = buildHeader(SYNC_BYTES, msgId, totalLength-(HEADER_SIZE+FOOTER_SIZE));
	buffer.set(headerBytes, offset);
	offset += headerBytes.length;
	view.setInt32(offset, value, true);
	offset += 4;
	const footerBytes = buildFooter(0);
	buffer.set(footerBytes, offset);
	return buffer;
}

// S32Message message parser
function parseS32Message(buffer){
	const dv = new DataView(buffer);
	let offset = 0;
	const syncBytes = dv.getUint32(offset, true);
	offset += 4;
	const messageType = dv.getUint16(offset, true);
	offset += 2;
	const bodySize = dv.getUint16(offset, true);
	offset += 2;
	const value = dv.getInt32(offset, true);
	offset += 4;
	const checksum = dv.getUint16(offset, true);
	offset += 2;
	return {
	messageType, bodySize, 
value
	, checksum
	};
}
// DoubleMessage message builder
function buildDoubleMessage(msgId, value) {
	const totalLength = 18;
	let offset = 0;
	const buffer = new Uint8Array(totalLength);
	const view = new DataView(buffer.buffer);
	const headerBytes = buildHeader(SYNC_BYTES, msgId, totalLength-(HEADER_SIZE+FOOTER_SIZE));
	buffer.set(headerBytes, offset);
	offset += headerBytes.length;
	view.setFloat32(offset, value, true);
	offset += 8;
	const footerBytes = buildFooter(0);
	buffer.set(footerBytes, offset);
	return buffer;
}

// DoubleMessage message parser
function parseDoubleMessage(buffer){
	const dv = new DataView(buffer);
	let offset = 0;
	const syncBytes = dv.getUint32(offset, true);
	offset += 4;
	const messageType = dv.getUint16(offset, true);
	offset += 2;
	const bodySize = dv.getUint16(offset, true);
	offset += 2;
	const value = dv.getFloat32(offset, true);
	offset += 8;
	const checksum = dv.getUint16(offset, true);
	offset += 2;
	return {
	messageType, bodySize, 
value
	, checksum
	};
}
// StringMessage message builder
function buildStringMessage(msgId, value) {
	const totalLength = 42;
	let offset = 0;
	const buffer = new Uint8Array(totalLength);
	const view = new DataView(buffer.buffer);
	const headerBytes = buildHeader(SYNC_BYTES, msgId, totalLength-(HEADER_SIZE+FOOTER_SIZE));
	buffer.set(headerBytes, offset);
	offset += headerBytes.length;
	buffer.set(value, offset);
	offset += 32;
	const footerBytes = buildFooter(0);
	buffer.set(footerBytes, offset);
	return buffer;
}

// StringMessage message parser
function parseStringMessage(buffer){
	const dv = new DataView(buffer);
	let offset = 0;
	const syncBytes = dv.getUint32(offset, true);
	offset += 4;
	const messageType = dv.getUint16(offset, true);
	offset += 2;
	const bodySize = dv.getUint16(offset, true);
	offset += 2;
	const value = new Uint8Array(dv.buffer, offset, 32);
	offset += 32;
	const checksum = dv.getUint16(offset, true);
	offset += 2;
	return {
	messageType, bodySize, 
value
	, checksum
	};
}
parseVersionMessage = function(buffer){
	return parseU32Message(buffer);
};

buildVersionMessage = function(msgId, value){
	return buildU32Message(msgId, value);
};

parseI2CAddressMessage = function(buffer){
	return parseU8Message(buffer);
};

buildI2CAddressMessage = function(msgId, value){
	return buildU8Message(msgId, value);
};

parseEthernetAddressMessage = function(buffer){
	return parseU32Message(buffer);
};

buildEthernetAddressMessage = function(msgId, value){
	return buildU32Message(msgId, value);
};

parseEthernetPortMessage = function(buffer){
	return parseU32Message(buffer);
};

buildEthernetPortMessage = function(msgId, value){
	return buildU32Message(msgId, value);
};

// MacAddressMessage message builder
function buildMacAddressMessage(msgId, mac) {
	const totalLength = 16;
	let offset = 0;
	const buffer = new Uint8Array(totalLength);
	const view = new DataView(buffer.buffer);
	const headerBytes = buildHeader(SYNC_BYTES, msgId, totalLength-(HEADER_SIZE+FOOTER_SIZE));
	buffer.set(headerBytes, offset);
	offset += headerBytes.length;
	buffer.set(mac, offset);
	offset += 6;
	const footerBytes = buildFooter(0);
	buffer.set(footerBytes, offset);
	return buffer;
}

// MacAddressMessage message parser
function parseMacAddressMessage(buffer){
	const dv = new DataView(buffer);
	let offset = 0;
	const syncBytes = dv.getUint32(offset, true);
	offset += 4;
	const messageType = dv.getUint16(offset, true);
	offset += 2;
	const bodySize = dv.getUint16(offset, true);
	offset += 2;
	const mac = new Uint8Array(dv.buffer, offset, 6);
	offset += 6;
	const checksum = dv.getUint16(offset, true);
	offset += 2;
	return {
	messageType, bodySize, 
mac
	, checksum
	};
}
parseSaveConfigurationMessage = function(buffer){
	return parseU8Message(buffer);
};

buildSaveConfigurationMessage = function(msgId, value){
	return buildU8Message(msgId, value);
};

parseLedStateMessage = function(buffer){
	return parseU8Message(buffer);
};

buildLedStateMessage = function(msgId, value){
	return buildU8Message(msgId, value);
};

// LedColorMessage message builder
function buildLedColorMessage(msgId, ledColor) {
	const totalLength = 13;
	let offset = 0;
	const buffer = new Uint8Array(totalLength);
	const view = new DataView(buffer.buffer);
	const headerBytes = buildHeader(SYNC_BYTES, msgId, totalLength-(HEADER_SIZE+FOOTER_SIZE));
	buffer.set(headerBytes, offset);
	offset += headerBytes.length;
	buffer.set(ledColor, offset);
	offset += 3;
	const footerBytes = buildFooter(0);
	buffer.set(footerBytes, offset);
	return buffer;
}

// LedColorMessage message parser
function parseLedColorMessage(buffer){
	const dv = new DataView(buffer);
	let offset = 0;
	const syncBytes = dv.getUint32(offset, true);
	offset += 4;
	const messageType = dv.getUint16(offset, true);
	offset += 2;
	const bodySize = dv.getUint16(offset, true);
	offset += 2;
	const ledColor = new Uint8Array(dv.buffer, offset, 3);
	offset += 3;
	const checksum = dv.getUint16(offset, true);
	offset += 2;
	return {
	messageType, bodySize, 
ledColor
	, checksum
	};
}
parseHomeDirectionMessage = function(buffer){
	return parseS8Message(buffer);
};

buildHomeDirectionMessage = function(msgId, value){
	return buildS8Message(msgId, value);
};

parseHomeSpeedMessage = function(buffer){
	return parseU32Message(buffer);
};

buildHomeSpeedMessage = function(msgId, value){
	return buildU32Message(msgId, value);
};

parseHomeThresholdMessage = function(buffer){
	return parseU32Message(buffer);
};

buildHomeThresholdMessage = function(msgId, value){
	return buildU32Message(msgId, value);
};

parseHomedStateMessage = function(buffer){
	return parseU8Message(buffer);
};

buildHomedStateMessage = function(msgId, value){
	return buildU8Message(msgId, value);
};

parseHomeMessage = function(buffer){
	return parseU32Message(buffer);
};

buildHomeMessage = function(msgId, value){
	return buildU32Message(msgId, value);
};

parseMotorStateMessage = function(buffer){
	return parseU8Message(buffer);
};

buildMotorStateMessage = function(msgId, value){
	return buildU8Message(msgId, value);
};

parseMotorBrakeMessage = function(buffer){
	return parseU8Message(buffer);
};

buildMotorBrakeMessage = function(msgId, value){
	return buildU8Message(msgId, value);
};

parseMaxSpeedMessage = function(buffer){
	return parseU32Message(buffer);
};

buildMaxSpeedMessage = function(msgId, value){
	return buildU32Message(msgId, value);
};

parseAccelerationMessage = function(buffer){
	return parseU32Message(buffer);
};

buildAccelerationMessage = function(msgId, value){
	return buildU32Message(msgId, value);
};

parseCurrentPositionMessage = function(buffer){
	return parseDoubleMessage(buffer);
};

buildCurrentPositionMessage = function(msgId, value){
	return buildDoubleMessage(msgId, value);
};

parseTargetPositionMessage = function(buffer){
	return parseDoubleMessage(buffer);
};

buildTargetPositionMessage = function(msgId, value){
	return buildDoubleMessage(msgId, value);
};

parseRelativeTargetPositionMessage = function(buffer){
	return parseDoubleMessage(buffer);
};

buildRelativeTargetPositionMessage = function(msgId, value){
	return buildDoubleMessage(msgId, value);
};

parseVelocityMessage = function(buffer){
	return parseDoubleMessage(buffer);
};

buildVelocityMessage = function(msgId, value){
	return buildDoubleMessage(msgId, value);
};

// VelocityAndStepsMessage message builder
function buildVelocityAndStepsMessage(msgId, velocity, steps, positionMode) {
	const totalLength = 19;
	let offset = 0;
	const buffer = new Uint8Array(totalLength);
	const view = new DataView(buffer.buffer);
	const headerBytes = buildHeader(SYNC_BYTES, msgId, totalLength-(HEADER_SIZE+FOOTER_SIZE));
	buffer.set(headerBytes, offset);
	offset += headerBytes.length;
	view.setInt32(offset, velocity, true);
	offset += 4;
	view.setInt32(offset, steps, true);
	offset += 4;
	view.setUint8(offset, positionMode, true);
	offset += 1;
	const footerBytes = buildFooter(0);
	buffer.set(footerBytes, offset);
	return buffer;
}

// VelocityAndStepsMessage message parser
function parseVelocityAndStepsMessage(buffer){
	const dv = new DataView(buffer);
	let offset = 0;
	const syncBytes = dv.getUint32(offset, true);
	offset += 4;
	const messageType = dv.getUint16(offset, true);
	offset += 2;
	const bodySize = dv.getUint16(offset, true);
	offset += 2;
	const velocity = dv.getInt32(offset, true);
	offset += 4;
	const steps = dv.getInt32(offset, true);
	offset += 4;
	const positionMode = dv.getUint8(offset, true);
	offset += 1;
	const checksum = dv.getUint16(offset, true);
	offset += 2;
	return {
	messageType, bodySize, 
velocity	, steps	, positionMode
	, checksum
	};
}
parseStartPathMessage = function(buffer){
	return parseU8Message(buffer);
};

buildStartPathMessage = function(msgId, value){
	return buildU8Message(msgId, value);
};

parseHAEnableMessage = function(buffer){
	return parseU8Message(buffer);
};

buildHAEnableMessage = function(msgId, value){
	return buildU8Message(msgId, value);
};

parseHAModeMessage = function(buffer){
	return parseU8Message(buffer);
};

buildHAModeMessage = function(msgId, value){
	return buildU8Message(msgId, value);
};

// HAIpAddressMessage message builder
function buildHAIpAddressMessage(msgId, ha_ip_address) {
	const totalLength = 14;
	let offset = 0;
	const buffer = new Uint8Array(totalLength);
	const view = new DataView(buffer.buffer);
	const headerBytes = buildHeader(SYNC_BYTES, msgId, totalLength-(HEADER_SIZE+FOOTER_SIZE));
	buffer.set(headerBytes, offset);
	offset += headerBytes.length;
	buffer.set(ha_ip_address, offset);
	offset += 4;
	const footerBytes = buildFooter(0);
	buffer.set(footerBytes, offset);
	return buffer;
}

// HAIpAddressMessage message parser
function parseHAIpAddressMessage(buffer){
	const dv = new DataView(buffer);
	let offset = 0;
	const syncBytes = dv.getUint32(offset, true);
	offset += 4;
	const messageType = dv.getUint16(offset, true);
	offset += 2;
	const bodySize = dv.getUint16(offset, true);
	offset += 2;
	const ha_ip_address = new Uint8Array(dv.buffer, offset, 4);
	offset += 4;
	const checksum = dv.getUint16(offset, true);
	offset += 2;
	return {
	messageType, bodySize, 
ha_ip_address
	, checksum
	};
}
parseHAVelocitySwitchOnSpeedMessage = function(buffer){
	return parseDoubleMessage(buffer);
};

buildHAVelocitySwitchOnSpeedMessage = function(msgId, value){
	return buildDoubleMessage(msgId, value);
};

parseHAVelocitySwitchOffSpeedMessage = function(buffer){
	return parseDoubleMessage(buffer);
};

buildHAVelocitySwitchOffSpeedMessage = function(msgId, value){
	return buildDoubleMessage(msgId, value);
};

parseHAPositionSwitchOnPositionMessage = function(buffer){
	return parseDoubleMessage(buffer);
};

buildHAPositionSwitchOnPositionMessage = function(msgId, value){
	return buildDoubleMessage(msgId, value);
};

parseHAPositionSwitchOffPositionMessage = function(buffer){
	return parseDoubleMessage(buffer);
};

buildHAPositionSwitchOffPositionMessage = function(msgId, value){
	return buildDoubleMessage(msgId, value);
};

parseHAVelocitySliderMinMessage = function(buffer){
	return parseDoubleMessage(buffer);
};

buildHAVelocitySliderMinMessage = function(msgId, value){
	return buildDoubleMessage(msgId, value);
};

parseHAVelocitySliderMaxMessage = function(buffer){
	return parseDoubleMessage(buffer);
};

buildHAVelocitySliderMaxMessage = function(msgId, value){
	return buildDoubleMessage(msgId, value);
};

parseHAPositionSliderMinMessage = function(buffer){
	return parseDoubleMessage(buffer);
};

buildHAPositionSliderMinMessage = function(msgId, value){
	return buildDoubleMessage(msgId, value);
};

parseHAPositionSliderMaxMessage = function(buffer){
	return parseDoubleMessage(buffer);
};

buildHAPositionSliderMaxMessage = function(msgId, value){
	return buildDoubleMessage(msgId, value);
};

parseHAMqttUserMessage = function(buffer){
	return parseStringMessage(buffer);
};

buildHAMqttUserMessage = function(msgId, value){
	return buildStringMessage(msgId, value);
};

parseHAMqttPasswordMessage = function(buffer){
	return parseStringMessage(buffer);
};

buildHAMqttPasswordMessage = function(msgId, value){
	return buildStringMessage(msgId, value);
};

parseHAMqttNameMessage = function(buffer){
	return parseStringMessage(buffer);
};

buildHAMqttNameMessage = function(msgId, value){
	return buildStringMessage(msgId, value);
};

parseHAMqttIconMessage = function(buffer){
	return parseStringMessage(buffer);
};

buildHAMqttIconMessage = function(msgId, value){
	return buildStringMessage(msgId, value);
};

