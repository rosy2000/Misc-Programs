#include <SoftwareSerial.h>
#define note_a_pin 2
#define note_b_pin 3
#define note_c_pin 4 // Pin assigned for each note
#define note_d_pin 5
#define note_e_pin 6
#define speaker_pin 7
#define rxPin_receive_player2 8
#define txPin_receive_player2 9
#define rxPin_receive_player1 10 // For receiving
#define txPin_receive_player1 11 // For transmitting
#define rxPin_send_player2 12
#define txPin_send_player2 13

#define note_delay 2.5 // 2.5 seconds delay between each note
#define next_round_delay 15 // 15 seconds delay between each round
#define max_retries 10 // Number of times master will ask slave to send score if not received

bool game_started;
float player1_score, player2_score;
String note_sequence_array[5];
String note_sequence; // The note sequence to be played is hardcoded in the app
int current_note_index; // The index of the current note that is being played

SoftwareSerial player1Response(rxPin_receive_player1, txPin_receive_player1);
SoftwareSerial player2Response(rxPin_receive_player2, txPin_receive_player2);
SoftwareSerial player2Send(rxPin_send_player2, txPin_send_player2);

void displayRoundEnd() {
  // To signify round end we will glow all the leds together and toggle it for 0.3 seconds
  int count = 0;
  while(count!=4) {
    if(count%2==0) {
      digitalWrite(note_a_pin, HIGH); // Glow all the LEDs
      digitalWrite(note_b_pin, HIGH);
      digitalWrite(note_c_pin, HIGH);
      digitalWrite(note_d_pin, HIGH);
      digitalWrite(note_e_pin, HIGH);
    } else {
        digitalWrite(note_a_pin, LOW); // Glow all the LEDs
        digitalWrite(note_b_pin, LOW);
        digitalWrite(note_c_pin, LOW);
        digitalWrite(note_d_pin, LOW);
        digitalWrite(note_e_pin, LOW);
    }
    count++;
    delay(300);
  }
}

void getScores() {
  // First we will get score from player 1 and then from player 2
  while(true) // Try again and again to write P
  {
    if(Serial.availableForWrite() >=1 ) { // If enough buffer size is available for serial to write then add 'P' to buffer
      Serial.write('P');
      break;
    }
  }
  int tries = 0;
  while(tries < max_retries) {
    player1Response.flush(); // Wait for the slaveResponse buffer to be completely empty before executing the below codes
    if(player1Response.available() >= 1) {
      player1_score = player1Response.read(); // Read the incoming byte and update player1_score
      break;
    }
    tries++; // Increment the number of loops
    delay(5);
  }
  while(true) // Try again and again to write P
  {
    if(player2Send.availableForWrite() >=1 ) { // If enough buffer size is available for serial to write then add 'P' to buffer
      player2Send.write('P');
      break;
    }
  }
  tries = 0;
  while(tries < max_retries) {
    player2Response.flush(); // Wait for the slaveResponse buffer to be completely empty before executing the below codes
    if(player2Response.available() >= 1) {
      player2_score = player2Response.read(); // Read the incoming byte and update player2_score
      break;
    }
    tries++; // Increment the number of loops
    delay(5);
  }
}

void displayResult() {
  float difference = player1_score - player2_score;
  if(difference > 0.0f) {
    while(true) {
      if(Serial.availableForWrite() >=1 ) {
        Serial.write("W"); // Player 1 won the game
        break;
      }
    }
  } else if (difference == 0.0f) {
    while(true) {
      if(Serial.availableForWrite() >= 1 && player2Send.availableForWrite() >= 1) { // We do this in one if-else to display the winner almost at the same time
        player2Send.write("W"); // Tie game
        Serial.write("W");
        break;
      }
    }
  } else {
    while(true) {
      if(player2Send.availableForWrite() >=1 ) {
        player2Send.write("W"); // Player 2 won the game
        break;
      }
    }
  }
  delay(3000); // Display the winner for 3 seconds
  while(true) {
    if(Serial.availableForWrite() >= 1 && player2Send.availableForWrite() >= 1) {
      Serial.write("R"); // Reset the players
      player2Send.write("R");
      break;
    }
  }
  delay(next_round_delay * 1000);
  game_started = false;
  current_note_index = 0;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // Start the connection at 9600 bauds : Default is 8 bauds 
  while(!Serial); // Wait for serial connection to establish
  pinMode(note_a_pin, OUTPUT);
  pinMode(note_b_pin, OUTPUT);
  pinMode(note_c_pin, OUTPUT);
  pinMode(note_d_pin, OUTPUT);
  pinMode(note_e_pin, OUTPUT);
  player2Send.begin(9600);
  while(!player2Send); // Wait for serial connection to be established
  player1Response.begin(9600);
  while(!player1Response);
  player2Response.begin(9600);
  while(!player2Response);
  game_started = false;
  player1_score = 0.0f;
  player2_score = 0.0f;
  note_sequence_array[0] = "ABABCDDABEEBACDBEE";
  note_sequence_array[1] = "ADDDCBACBDCCDABBAC"; 
  note_sequence_array[2] = "BBCDABCBDBCBABCEEE";
  note_sequence_array[3] = "CCBABCBDBEABCBEBCE"; // Sequence of random notes;
  note_sequence_array[4] = "DACBCABDEBACBDEABC";
  note_sequence_array[5] = "EABCBDEBACADEBBBEA";
  randomSeed(analogRead(0)); 
  // Establish a random seed with a value which will continuosly change over time. analogRead(0) 
  //checks for input on pin 0. On not finding one, garbage value is returned
  current_note_index = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!game_started) {
    // Initially play the notes, and glow the corresponding LED
    int arr[] = {note_a_pin, note_b_pin, note_c_pin, note_d_pin, note_e_pin};
    int signal_power_counter = 1; // Use this to increment the signal strength
    int array_length = sizeof(arr)/sizeof(arr[0]);
    for(int i = 0; i<array_length; i++) {
      tone(speaker_pin, 1000 * signal_power_counter++); // Send 1KHz sound signal
      digitalWrite(arr[i], HIGH); // Glow the corresponding LED for that note which points to the required button
      delay(1500); // Play the sound for 1.5 seconds
      digitalWrite(arr[i], LOW); // Turn of the LED
      noTone(speaker_pin); // Stop the sound
      delay(1000); // Wait for another 1000 seconds before playing the next tone
    game_started = true; // Now the game will be started
    delay(500);
    long random_no = random(0,5);
    note_sequence = note_sequence_array[random_no]; // Select one of the five notes available
    }
  } else {
    if(current_note_index == sizeof(note_sequence)) {
      // If we have finished all the notes then the game must end
      displayRoundEnd();
      getScores();
      displayResult();
    } else {
       char current_note = note_sequence[current_note_index];
       if(Serial.availableForWrite() >=1 && player2Send.availableForWrite() >= 1) { // Char is 1 bytes so only check for 1 byte
        // Serial.write will block program execution until there is enough space in  buffer. To prevent that check how much byte is available
        Serial.write(current_note);
        player2Send.write(current_note); // Send the current note to both the player
        current_note_index++;
        delay(note_delay * 1000); // Wait for 2.5 seconds before starting next note
       }
     }
  }
}

  
