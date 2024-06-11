// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
// rs = PD7 , en = PD6 , d4 = PD5 , d5 = PD4 , d6 = PD3 , d7 = PD2
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define led (1 << PB3)
#define botaoPressao (1<<PC2)

int forcaBotao = 0;
int contSegundo = 0;
int contTempo = 10;
int limiarForca = 126;

// Conta o tempo só se a força tiver chegado a um limiar de força
ISR(TIMER0_COMPA_vect){
  if(forcaBotao >= limiarForca){
    contSegundo++;
  }
  // conta 1 segundo ,reseta o contSegundo e diminui 1 do contTempo
  if(contSegundo >= 10000){
    contTempo--;
    contSegundo=0;

    // verifica se o contador está em 9, se estiver limpar o digito 0 (do numero anterior, no caso 10)
    if(contTempo==9){
      lcd.setCursor(1, 1);
      lcd.print(" ");
    }

    // n deixa a contagem ficar negativa e recomeça ela
    if(contTempo==-1){
      contTempo=10;
    }

    // seta o cursor para a segunda linha, primeira coluna e mostra o contTempo
    lcd.setCursor(0, 1);
    lcd.print(contTempo);
  }
}

int main(){

  // CONFIGURAÇÃO DO CONVERSOR A/D  
  ADMUX = 0;
  ADMUX |= (1<<MUX1); //SELECIONA A ENTRADA PC2
  ADMUX |= (1<<REFS0); //TENSÃO DE REFERÊNCIA
  ADCSRA |= (1<<ADEN); //LIGA O CONVERSOR AD
  ADCSRA |= (1<<ADPS0) | (1<<ADPS1); //PRESCALER = 8
  ADCSRA |= (1<<ADSC); //INICIA A CONVERSÃO

  DDRB |= led;
  DDRC |= botaoPressao;

  // Configuração do timer
  // Configura o modo de operação, nesse caso por comparação
  TCCR0A |= (1<<WGM01);
  // Configura o Pré-Scaler de 8
  TCCR0B |= (1<<CS01);
  // Define o limiar de contagem do modo comparação
  OCR0A = 200; // cont = 10000 - 1s
  // Liga a interrupção do comparador A do timer0
  TIMSK0 = (1<<OCIE0A);

  // CONFIGURAÇÃO DO PWM    
  // TCCR2A = (1<<COM2A1); //DESLIGA O PINO NA IGUALDADE COM OCR
  // TCCR2A |= (1<<WGM21) | (1<<WGM20); // MODO PWM RÁPIDO
  // TCCR2B = (1<<CS20) | (1<<CS22);
  // OCR2A = 102;

  sei();

  // set up the LCD's number of columns and rows:
  // primeiro digito é o numero de colunas, segundo digito é o numero linhas
  lcd.begin(16, 2);
  // Printa o "Mantenha por" na linha 0 e coluna 0
  lcd.print("Mantenha por");
  // Printa o "10" na linha 1 e coluna 0
  lcd.setCursor(0, 1);
  lcd.print("10");
  // Printa o "segundos" na linha 1 e coluna 4
  lcd.setCursor(4, 1);
  lcd.print("segundos");

  while(1){
    while (ADCSRA&(1<<ADSC));

    // função para traduzir o numero de 10 bits para 8 bits
    forcaBotao = map (ADC,0,1023,0,255);

    // ativa a conversão
    ADCSRA |= (1<<ADSC);
    
    // Ascende o led se a força do botao chegar no limiar
    if(forcaBotao >= limiarForca){
      PORTB |= led;
    }
    // Apaga o led se a força não chegar no limiar
    else{
      PORTB &= !(led);
    }
    
  }
}