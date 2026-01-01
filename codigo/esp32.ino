#include <Math.h> // Biblioteca matemática
#include <WiFiUdp.h> // Comunicação UDP (usada para NTP)
#include <NTPClient.h> // Cliente NTP para obter horário
#include <ArduinoJson.h> // Manipulação de JSON (Firebase)
#include <IOXhop_FirebaseESP32.h> // Conexão com Firebase
#include <WiFi.h> // Conexão Wi-Fi

// Dados do Firebase
#define FIREBASE_HOST "https://lixeira-inteligente-14cf1-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "6qxz0tCh0j8Z6AL9bCVrtRW2G0MgeynCkZZhCEke"

// Dados da rede Wi-Fi
const char* ssid = "LDI2";
const char* pass = "wifi2021";

// Objetos para NTP e controle de tempo
WiFiUDP ntpUDP;
NTPClient cliente_time(ntpUDP, "pool.ntp.org", -10800); // Fuso horário -3h (Brasil)

// Variáveis para controle da tampa via app
String abrir_tampa;
String abrir_tampa_ult;
String msg_ant; // Msg anterior do arduino

void setup()
{
    Serial.begin(9600); // Comunicação com o PC
    Serial2.begin(9600, SERIAL_8N1, 16, 17); // Comunicação com Arduino via pinos 16 e 17

    // Conectando ao Wi-Fi
    Serial.println("conectando ao wifi ");
    Serial.print(ssid);
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println(" conectado!");
    msg_ant = "";

    // Conectando ao Firebase
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

    // Obtém o último comando de abertura da tampa
    abrir_tampa_ult = Firebase.getString("abrir_tampa");
    abrir_tampa_ult.trim();

    // Inicializa o cliente NTP
    cliente_time.begin();
}

void loop()
{
    cliente_time.update(); // Atualiza tempo

    if (Serial2.available())
    {
        String msg = Serial2.readStringUntil('\n'); // lê o arduino
        msg_ant = Firebase.getString("nivel_lixeira");
        msg.trim();
        Serial.println(msg);

        if (msg.indexOf("Abriu") >= 0 && msg_ant.indexOf("Abriu") < 0) // Se o arduino abriu
        {
            // Atualizando a media ponderada dos picos
            String horas = Firebase.getString("horas_pico");
            horas.trim();

            if (horas.length() > 46)
            {
                int vet[24];

                for (int i = 0; i < 24; i++)
                {
                    int pos = horas.indexOf("-");

                    if (pos >= 0)
                    {
                        vet[i] = horas.substring(0, pos).toInt();
                        horas.remove(0, pos + 1);
                    }
                    else
                    {
                        vet[i] = 0;
                    }
                }

                vet[cliente_time.getHours()]++;
                horas = "";

                for (int i = 0; i < 24; i++)
                {
                    horas += String(vet[i]) + "-";
                }

                Firebase.setString("horas_pico", horas);

                float media = 0;
                float dem = 0;

                for (int i = 0; i < 24; i++)
                {
                    media += vet[i] * i;
                    dem += vet[i];
                }

                media = (dem == 0) ? 0 : media / dem;
                int media_int = round(media);
                Firebase.setInt("media_pico", media_int);
            }
        }
        else if (msg.indexOf("Cheio") >= 0 && msg_ant.indexOf("Cheio") < 0)
        {
            Firebase.setString("nivel_lixeira", "Cheio");

            // Atualizando a moda das cheias
            String horas = Firebase.getString("horas_cheia");
            horas.trim();

            if (horas.length() > 46)
            {
                int vet[24];

                for (int i = 0; i < 24; i++)
                {
                    int pos = horas.indexOf("-");

                    if (pos >= 0)
                    {
                        vet[i] = horas.substring(0, pos).toInt();
                        horas.remove(0, pos + 1);
                    }
                    else
                    {
                        vet[i] = 0;
                    }
                }

                vet[cliente_time.getHours()]++;
                horas = "";

                for (int i = 0; i < 24; i++)
                {
                    horas += String(vet[i]) + "-";
                }

                Firebase.setString("horas_cheia", horas);

                int moda = 0;
                int index_moda = 0;

                for (int i = 0; i < 24; i++)
                {
                    if (vet[i] >= moda)
                    {
                        moda = vet[i];
                        index_moda = i;
                    }
                }

                Firebase.setInt("media_cheia", index_moda);
            }
        }
        else if (msg.indexOf("Medio") >= 0 && msg_ant.indexOf("Medio") < 0)
        {
            Firebase.setString("nivel_lixeira", "Medio");
        }
        else if (msg.indexOf("Baixo") >= 0 && msg_ant.indexOf("Baixo") < 0)
        {
            Firebase.setString("nivel_lixeira", "Baixo");
        }
    }

    abrir_tampa = Firebase.getString("abrir_tampa");
    abrir_tampa.trim();

    if (abrir_tampa != abrir_tampa_ult)
    {
        if (abrir_tampa.indexOf("Abrir") >= 0)
        {
            Serial2.println("Abrir");
        }
        else
        {
            Serial2.println("Fechar");
        }

        abrir_tampa_ult = abrir_tampa;
    }

    delay(50);
}
