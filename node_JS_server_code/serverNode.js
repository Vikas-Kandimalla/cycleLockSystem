

const SerialPort = require("serialport");
var admin = require("firebase-admin");


var serviceAccount = require("./serviceAccountKey.json");

admin.initializeApp({
    credential: admin.credential.cert(yourServiceAccount), // Replace with your service account. 
    databaseURL: "https://yourproject.firebaseio.com"   // Replace with your firebase link
  });





const db = admin.firestore();


var portName = process.argv[2];

if (portName == null || portName == ""){
    console.error("Error: port number not entered\n USAGE: node serverNode PORTNAME.\n eg: node serverNode COM4");
    process.exit(1);
}

const port = new SerialPort(portName,{
    baudRate : 9600
});

const parsers = SerialPort.parsers;



// Use a `\r\n` as a line terminator
const parser = new parsers.Readline({
  delimiter: '\r\n'
});
port.pipe(parser);


port.on('open', function() {
  console.log("Serial Port opened on " + portName);
});

port.on('error', function(err) {
    console.log('Error: ', err.message);
});
  
parser.on('data',readFunction);




function readFunction(data){
    if (data.indexOf("KEYUPDATE::") != -1){
        var i = data.indexOf("key=");
        var key = data.slice(i+4,i+8);

        i = data.indexOf("device=");
        var device = data.slice(i+7,i+9);      
        console.log("Key update requested. key = " + key, " device = " + device);
        updateKey(device,key);
    }
    else if (data.indexOf("KEYREQUEST::") != -1){
      
        var key = getRandomKey();
        port.write(key);

    }
    else if (data.indexOf("Press") != -1){
        console.log(data);
        port.write('\n');
    }
    else {
        console.log(data);
        
    }
}


function updateKey(device ,key){
   
    const i = {
        key : key
    };
    var name = getName(device);
    db.collection('passcodes').doc(name).set(i).then(() => {
        console.log("key updated in database. device = " + name + " key = " + key);
    }, function(err) {
        console.error("Cannot update key in database.Check your internet connection.");
    });
}


function getName(device){
    if (device == "97"){
        return "CY1234";
    }
    else 
        return "CY0000";
}

function getRandomKey() {
    var key = "";
    var n;
    for (var i = 0; i < 4; i++) {
        n =  Math.floor(Math.random() * 10);
        while (n == 10){
        n = Math.floor(Math.random() * 10);
        }
        key += n;
    }
    return key;
} 

function getRandomNumber() {
    return Math.floor(Math.random() * 10);
}
// open errors will be emitted as an error event 
