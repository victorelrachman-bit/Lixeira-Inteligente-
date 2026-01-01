#Lixeira Inteligente com IOT

##Objetivo
  A lixeira inteligente é capaz de:
  - Abrir e fechar automáticamente
  - Infromar o nível de lixo
  - Coletar dados de uso ao longo do tempo
  - Analisar os dados coletados

##Funcionamento
  - Um sensor, quando ativado, abre a tampa por 5 segundos com um servo motor
  - Outro sensor mede a distância da tampa ao lixo, para dizer o nível do lixo
  - A ESP32 envia dados já analisados para um banco de dados (Firebase)
  - O aplicativo mostra os dados e controla a lixeira remotamente

##Análise dos dados
  Os dados são analisados da seguinte forma:
  - Média ponderada dos horários de uso
  - Moda dos outros em que a lixeira ficou cheia

##Tecnologias utilizadas
  - Arduino
  - ESP32
  - 2 HC-SR04
  - Micro servo motor 9g
  - Tela LCD
  - Firebase
  - App inventor

##Aplicações
  A tecnologia pode ser aplicada em escolas, hoteis e espaços públicos, auxiliando na gestão de resíduos e tomada de decisões.

##Autor
  Victor Gabriel El Rachman Carvalho da Silva

  - 
