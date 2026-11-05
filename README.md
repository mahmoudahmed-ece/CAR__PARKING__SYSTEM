# CAR__PARKING__SYSTEM
Car parking sensor on ATmega32 in bare-metal C. Measures distance with an HC-SR04 using the Timer1 Input Capture Unit (0.5 µs resolution), shows it live on a 16x2 LCD, and warns with progressive LEDs and a buzzer. Layered drivers (MCAL / HAL / App): GPIO, ICU, Ultrasonic, LCD.
