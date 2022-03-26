/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-mpu-6050-web-server/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

let scene, camera, rendered, cube;

let players = ["Lara", "Ben", "Fabian", "Marvin"];
let currentPlayer = 0;

function parentWidth(elem) {
  return elem.parentElement.clientWidth - 40;
}

function parentHeight(elem) {
  return elem.parentElement.clientHeight - 40;
}

function init() {
  document.getElementById("current-player").innerHTML = players[currentPlayer];
}

function init3D(){
  scene = new THREE.Scene();
  scene.background = new THREE.Color(0xffffff);

  camera = new THREE.PerspectiveCamera(75, parentWidth(document.getElementById("3Dcube")) / parentHeight(document.getElementById("3Dcube")), 0.1, 1000);

  renderer = new THREE.WebGLRenderer({ antialias: true });
  renderer.setSize(parentWidth(document.getElementById("3Dcube")), parentHeight(document.getElementById("3Dcube")));

  document.getElementById('3Dcube').appendChild(renderer.domElement);

  // document.getElementById('3Dcube').style.width = "600px";
  
  // Create a geometry for cube
  const geometry = new THREE.BoxGeometry(5, 5, 5);

  //textures
  const loader = new THREE.TextureLoader();
  const cubeMaterials = [
      new THREE.MeshBasicMaterial({ map: loader.load('textures/nx.png') }), //right side
      new THREE.MeshBasicMaterial({ map: loader.load('textures/ny.png')}), //left side
      new THREE.MeshBasicMaterial({ map: loader.load('textures/nz.png')}), //top side
      new THREE.MeshBasicMaterial({ map: loader.load('textures/px.png')}), //bottom side
      new THREE.MeshBasicMaterial({ map: loader.load('textures/py.png')}), //front side
      new THREE.MeshBasicMaterial({ map: loader.load('textures/pz.png')}), //back side
  ];
 
  cube = new THREE.Mesh(geometry, cubeMaterials);
 
  scene.add(cube);
  
  camera.position.z = 10;
  renderer.render(scene, camera);
}

// Resize the 3D object when the browser window changes size
function onWindowResize(){
  camera.aspect = parentWidth(document.getElementById("3Dcube")) / parentHeight(document.getElementById("3Dcube"));
  //camera.aspect = window.innerWidth /  window.innerHeight;
  camera.updateProjectionMatrix();
  //renderer.setSize(window.innerWidth, window.innerHeight);
  renderer.setSize(parentWidth(document.getElementById("3Dcube")), parentHeight(document.getElementById("3Dcube")));

}

window.addEventListener('resize', onWindowResize, false);

// Create the 3D representation
init();
init3D();

function render() {
  renderer.render(scene, camera);
  requestAnimationFrame(render);
}
requestAnimationFrame(render);

// Create events for the sensor readings
if (!!window.EventSource) {
  var source = new EventSource('/events');

  source.addEventListener('open', function(e) {
    console.log("Events Connected");
  }, false);

  source.addEventListener('error', function(e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log("Events Disconnected");
    }
  }, false);

  source.addEventListener('gyro_readings', function(e) {
    //console.log("gyro_readings", e.data);
    var obj = JSON.parse(e.data);
    document.getElementById("gyroX").innerHTML = obj.gyroX;
    document.getElementById("gyroY").innerHTML = obj.gyroY;
    document.getElementById("gyroZ").innerHTML = obj.gyroZ;

    // Change cube rotation after receiving the readinds
    cube.rotation.x = obj.gyroY;
    cube.rotation.z = obj.gyroX;
    cube.rotation.y = obj.gyroZ;
    renderer.render(scene, camera);
  }, false);

  source.addEventListener('temperature_reading', function(e) {
    console.log("temperature_reading", e.data);
    document.getElementById("temp").innerHTML = e.data;
  }, false);

  source.addEventListener('accelerometer_readings', function(e) {
    console.log("accelerometer_readings", e.data);
    var obj = JSON.parse(e.data);
    document.getElementById("accX").innerHTML = obj.accX;
    document.getElementById("accY").innerHTML = obj.accY;
    document.getElementById("accZ").innerHTML = obj.accZ;
  }, false);

  source.addEventListener('accelerometer_movement', function(e) {
    console.log("accelerometer_movement", e.data);
    var mov = e.data;
    document.getElementById("accMov").innerHTML = mov;
  }, false);

  source.addEventListener('dice_result', function(e) {
    console.log("dice_result", e.data);
    var result = e.data;
    addDiceResult(currentPlayer, result);
    let playerCount = players.length;
    currentPlayer = (currentPlayer+1) % playerCount;
    document.getElementById("current-player").innerHTML = players[currentPlayer];
  }, false);
}

function resetPosition(element){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/"+element.id, true);
  console.log(element.id);
  xhr.send();
}

function calibrateGyro(){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/calibrateGyro", true);
  console.log("Calibrate gyro");
  xhr.send();
}

function calibrateAcc(){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/calibrateAcc", true);
  console.log("Calibrate acc");
  xhr.send();
}

function addDiceResult(currentPlayer, result) {
  let row = "<tr><td>" + players[currentPlayer] + "</td><td>" + result + "</td></tr>";
  document.getElementById("tbody").innerHTML = row + document.getElementById("tbody").innerHTML;
}