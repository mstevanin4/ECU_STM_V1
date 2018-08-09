
#define PIN_CRK 12
#define T_PER_TOOTH 1000

int tooth = 0;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  pinMode(PIN_CRK, OUTPUT);
}

void loop ()
{
    digitalWrite(PIN_CRK, HIGH);
    delayMicroseconds(T_PER_TOOTH);
    digitalWrite(PIN_CRK, LOW);
    delayMicroseconds(T_PER_TOOTH);
}
