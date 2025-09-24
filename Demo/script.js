var ph=["#ed1b26","#f46432","#f78f1e","#ffc324","#fef200","#84c341","#4db749","#33a94b","#0ab8b6","#4690cd","#3853a4","#5a51a2","#63459d","#6c2180","#49176e"];

var noir="#1e272e";
var blanc="#d2dae2";

const temperature = document.getElementsByClassName("Temperature");
const PH = document.getElementsByClassName("PH");
const lumiere = document.getElementsByClassName("Lumiere");
let iPH = 0;

function changerTemperature(){
	let nombreAleatoire = Math.floor(Math.random() * 11) + 20;

	if(nombreAleatoire<25){
		temperature[0].firstElementChild.firstElementChild.style.fill ="#1d4fff";
	}
	else if (nombreAleatoire>27){
		temperature[0].firstElementChild.firstElementChild.style.fill="#fd2a29";
	}
	else
	temperature[0].firstElementChild.firstElementChild.style.fill ="#2caf49";

	temperature[0].firstElementChild.lastElementChild.innerHTML= `${nombreAleatoire}°C`
}

function changerLumiere(etat){
	if(etat==0){
		lumiere[0].firstElementChild.firstElementChild.style.fill ="white";
		lumiere[0].firstElementChild.lastElementChild.style.fill="black";
	}
	else{
		lumiere[0].firstElementChild.firstElementChild.style.fill ="black";
		lumiere[0].firstElementChild.lastElementChild.style.fill="white";
	}
}

function changerPH(){
	if (iPH == 14){
			iPH = 0;
		}
		PH[0].firstElementChild.firstElementChild.style.fill=ph[iPH];
		PH[0].firstElementChild.lastElementChild.innerHTML= `PH:${iPH}`

		iPH++;
}

$(document).ready(function() {
	let etatLumiere = 0
	changerPH()
	changerTemperature()
	setInterval(()=>{
		changerPH()
		changerTemperature()
	},1000);
	
	
var lumiereSvg = document.getElementById('lumiere-svg');


lumiereSvg.addEventListener('click', function() {
  etatLumiere = !etatLumiere
  changerLumiere(etatLumiere)
});

var nourriture = document.getElementById('nourriture-svg');

let compteur = 1;

nourriture.addEventListener('click', function() {
	compteur++;
	nourriture.lastElementChild.innerHTML = `${compteur} repas`
});

});