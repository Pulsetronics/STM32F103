
void I2Cx_Init(uint32_t _clockSpeed){
	 I2C_InitTypeDef OLED;
	 GPIO_InitTypeDef GPIO;
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	 GPIO_PinRemapConfig(Remap_I2Cx, ENABLE);

	 // Initialise I2C Remap GPIO pins
	 GPIO.GPIO_Mode  = GPIO_Mode_AF_OD;
	 GPIO.GPIO_Pin = I2C_PIN_SCL | I2C_PIN_SDA;
	 GPIO.GPIO_Speed = GPIO_Speed_50MHz;

	 GPIO_Init (GPIOB, &GPIO);
	 // Initialise I2C

	 OLED.I2C_Ack = I2C_Ack_Enable;
	 OLED.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	 OLED.I2C_DutyCycle = I2C_DutyCycle_2;
	 OLED.I2C_Mode = I2C_Mode_I2C;
	 OLED.I2C_OwnAddress1 = 0x00;
	 OLED.I2C_ClockSpeed = 100000;

	 I2C_Init(I2Cx, &OLED);
	 I2C_Cmd(I2Cx, ENABLE);

}

void I2C_Start(void){
	//Wait until I2Cx is not busy
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));

	//Generate start condition
	I2C_GenerateSTART(I2Cx, ENABLE);

	//Wait for I2C
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
}

void I2C_Stop(void){
	// Generate I2C stop condition
	I2C_GenerateSTOP(I2Cx, ENABLE);

	// Wait until I2C stop condition is finished
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF));
}

void I2C_Address_Direction(uint8_t _address, uint8_t _direction){
	// send slave address
	I2C_Send7bitAddress(I2Cx, _address, _direction);

	//wait
	if(_direction == I2C_Direction_Transmitter){
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	}
	else if (_direction == I2C_Direction_Receiver){
	     while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	}
}

void I2C_Transmit(uint8_t _byte){
	 //send data byte
	I2C_SendData(I2Cx, _byte);

	//wait
	//while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	while(!I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF));
}

uint8_t I2C_Receive_Ack(){
	// Enable ACK of received data
	I2C_AcknowledgeConfig(I2Cx, ENABLE);

	//Wait
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));

	//Read and return data byte from I2C data register
	return I2C_ReceiveData(I2Cx);
}

uint8_t I2C_Receive_Nack(){
	 // Disable ACK of received
	I2C_AcknowledgeConfig(I2Cx, DISABLE);

	//wait
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));

	//read and return data byte from I2C data register
	return I2C_ReceiverData(I2Cx);
}

void I2C_Write(uint8_t _address, uint8_t _data){
	 I2C_Start();
	// I2C_Address_Direction(_address << 1, I2C_Direction_Transmitter);
	 I2C_Send7bitAddress(I2Cx, _address, I2C_Direction_Transmitter);
	 		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	 I2C_Transmit(_data);
	 I2C_Stop();
}

void I2C_Read(uint8_t _address, uint8_t* data){
	 I2C_Start();
	 I2C_Address_Direction(_address << 1, I2C_Direction_Receiver);
	 *data = I2C_Receive_Nack();
	 I2C_Stop();
}
