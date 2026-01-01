#include <Ultrasonic.h> //biblioteca de ultrassom
#include <Servo.h> //biblioteca do servo
#include <LiquidCrystal_I2C.h> //biblioteca do lcd
#include <SoftwareSerial.h> //biblioteca pra conectar a esp32

//definindo pinos
#define servo_pin 8
#define trig_m 9
#define echo_m 10
#define trig_n 11
#define echo_n 12

//Variaveis e objetos
LiquidCrystal_I2C lcd(0x27, 16, 2); //DISPLAY LCD
SoftwareSerial esp(2, 3); //RX, TX
Servo servo; //Servo
Ultrasonic nivel(trig_n, echo_n); //ultrassom de nivel
Ultrasonic mao(trig_m, echo_m); //ultrasom de mao

unsigned long time_mill; //tempo em milissegundos que o arduino esta
funcionando
unsigned long time_mill_ult; //tempo em millisegundos desde que o servo
abriu

bool open; //Se esta aberto
bool open_app; //Se o app pediu pra abrir
String msg_lcd; //msg do lcd
int dis_m; //distancia da mao
int dis_n; //distancia do nivel
int delay_servo = 5000; //delay pro servo: 5s

void setup()
{
    esp.begin(9600); //inicializando conexao uart com a esp
    Serial.begin(9600);

    //Inicializando lcd
    lcd.init();
    lcd.backlight();

    //Inicializando o servo
    servo.attach(servo_pin);
    servo.write(90);

    //valores iniciais de variaveis
    open = false;
    open_app = false;
    time_mill = millis();
    time_mill_ult = millis();
    msg_lcd = "";
}

void loop()
{
    delay(1000);
    time_mill = millis(); //atualiza o tempo

    if (esp.available())
    {
        String msg = esp.readString(); //msg da esp
        msg.trim();

        if (msg.indexOf("Abrir") >= 0 && open_app == false) //se o app pediu pra abrir
        {
            servo.write(0);
            open_app = true;
            return;
        }
        else if (msg.indexOf("Abrir") >= 0 && open_app == true) //se o app pediu pra abrir
        {
            if (servo.read() != 0)
            {
                servo.write(0);
            }
            return;
        }
        else if (msg.indexOf("Fechar") >= 0 && open_app == true) //se o app pediu pra fechar
        {
            servo.write(90);
            open_app = false;
            Serial.println("2");
        }
        else if (msg.indexOf("Fechar") < 0 && open_app == true) //nenhuma das opcoes
        {
            if (servo.read() != 0)
            {
                servo.write(0);
            }
            return;
        }
    }

    if (!open_app)
    {
        dis_m = mao.read(); //lendo a distancia da mao

        if (dis_m < 0 || dis_m > 600)
        {
            return;
        }

        if (open == false && dis_m <= 14) //se a mao esta a menos de 5cm de
        distancia
        {
            open = true;
            esp.println("Abriu");
            servo.write(0);
            time_mill_ult = millis();
            return;
        }
        else if (open == true && time_mill - time_mill_ult <= delay_servo
                 && servo.read() != 0) //se o servo nao obedeceu
        {
            servo.write(0);
            return;
        }
        else if (open == true && time_mill - time_mill_ult > delay_servo)
        //se ja passou 5s desde que o servo abriu
        {
            open = false;
            servo.write(90);
            return;
        }
        else if (open == false && dis_m > 5 && time_mill - time_mill_ult >
                 delay_servo)
        {
            delay(100);
            dis_n = nivel.read();

            if (dis_n < 0 || dis_n > 20)
            {
                return;
            }

            //3 pra baixo - cheio
            //4 a 11 - medio
            //12 pra cima baixo

            if (dis_n <= 7 && dis_n >= 0)
            {
                if (msg_lcd != "Nivel: Cheio")
                {
                    lcd.clear();
                    msg_lcd = "Nivel: Cheio";
                    lcd.print(msg_lcd);
                    esp.println("Cheio");
                }
            }
            else if (dis_n > 7 && dis_n <= 12)
            {
                if (msg_lcd != "Nivel: Medio")
                {
                    lcd.clear();
                    msg_lcd = "Nivel: Medio";
                    lcd.print(msg_lcd);
                    esp.println("Medio");
                }
            }
            else if (dis_n > 12 && dis_n < 20)
            {
                if (msg_lcd != "Nivel: Baixo")
                {
                    lcd.clear();
                    msg_lcd = "Nivel: Baixo";
                    lcd.print(msg_lcd);
                    esp.println("Baixo");
                }
            }
        }
    }

    delay(1000);
}
