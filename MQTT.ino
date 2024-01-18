#include <ESP8266WiFi.h>                // Thư viện dùng để kết nối WiFi của ESP8266
#include <PubSubClient.h>               // Thư viện dùng để connect, publish/subscribe MQTT

const char* ssid = "TrThanh.69";         // Tên của mạng WiFi mà bạn muốn kết nối đến
const char* password = "donknow?";   // Mật khẩu của mạng WiFi

const char* mqttServer = "test.mosquitto.org";
const int   mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";

const int led = 2;                      // Đèn led ở chân GPIO2

char messageBuff[100];                  // Biến dùng để lưu nội dung tin nhắn

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);                 // Khởi tạo kết nối Serial để truyền dữ liệu đến máy tính

  pinMode(led, OUTPUT);

  startWiFi();

  connectBroker();

  client.subscribe("blockynode/led");                           // Đăng ký nhận tin từ topic = "blockynode/led"
  client.publish("blockynode/led", "Hello from Blocky Node");   // Gởi message = "Hello from Blocky Node" đến topic = "blockynode/led"

}

void loop() {
  if (!client.connected()) {
    connectBroker();
  }
  client.loop();
}

void startWiFi() {
  WiFi.begin(ssid, password);           // Kết nối vào mạng WiFi
  Serial.print("Connecting to ");
  Serial.print(ssid);
  // Chờ kết nối WiFi được thiết lập
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n");
  Serial.println("Connection established!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());       // Gởi địa chỉ IP đến máy tinh
}

void connectBroker() {
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("BlockyNodeClientID")) {    // Kêt nối đến broker thành công
      Serial.println("\n");
      Serial.println("MQTT connected");
    } else {
      Serial.println("\n");
      Serial.print("MQTT failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  int i;
  for (i = 0; i < length; i++) {
    messageBuff[i] = (char)payload[i];
  }
  messageBuff[i] = '\0';

  String message = String(messageBuff);
  Serial.println(message);

  if (message == "on") {
    digitalWrite(led, LOW);                       // Bật đèn LED khi nhận được tin nhắn "on"
    client.publish("blockynode/led", "LED is turned ON");   // Thông báo trạng thái bật LED cho tất cả client đăng ký topic = "blockynode/led"
  } else if (message == "off") {                  // Tắt đèn LED khi nhận được tin nhắn "off"
    digitalWrite(led, HIGH);
    client.publish("blockynode/led", "LED is turned OFF");  // Thông báo trạng thái tắt LED cho tất cả client đăng ký topic = "blockynode/led"
  }
}