const ph = ["#ed1b26", "#f46432", "#f78f1e", "#ffc324", "#fef200", "#84c341", "#4db749", "#33a94b", "#0ab8b6", "#4690cd", "#3853a4", "#5a51a2", "#63459d", "#6c2180", "#49176e"];

const noir = "#1e272e";
const blanc = "#d2dae2";
const url = "";

function RecupererEtats(){

		$.ajax({
		url: url+'etats',
		type: 'GET',
		dataType: 'json'
	})
	.done(function(etat) {
		if(etat.lumiere==0){
			$(".Lumiere path").attr('fill',blanc)
			$(".Lumiere text").attr('fill',noir)
		}
		else{
			$(".Lumiere path").attr('fill',noir)
			$(".Lumiere text").attr('fill',blanc)
		}

		$(".PH path").attr('fill', ph[Number((etat.ph).toFixed(0))]);
		$(".PH text").text("PH:"+Number((etat.ph).toFixed(1)))

		if(etat.temperature<25){
			$(".Temperature path").attr('fill', "#1d4fff");
		}
		else if (etat.temperature>27){
			$(".Temperature path").attr('fill', "#fd2a29");
		}
		else
			$(".Temperature path").attr('fill', "#2caf49");

		$(".Temperature text").text(etat.temperature+"°C")
		$(".Nourriture text").text(etat.repas+" repas")
	})
}

function GererLumiere(){
			$.ajax({
			url: url+'lumiere',type: 'GET'
		})
		.done(function(etatLumiere) {
			
			console.log(etatLumiere)
			if(etatLumiere==0){
				$(".Lumiere path").attr('fill',blanc)
				$(".Lumiere text").attr('fill',noir)
			}
			else{
				$(".Lumiere path").attr('fill',noir)
				$(".Lumiere text").attr('fill',blanc)
			}
		})
}

function RecupererTemperature(){
	$.ajax({
		url: url+'temperature',
		type: 'GET'
	})
	.done(function(temperature) {
		if(temperature<25){
			$(".Temperature path").attr('fill', "#1d4fff");
		}
		else if (temperature>27){
			$(".Temperature path").attr('fill', "#fd2a29");
		}
		else
			$(".Temperature path").attr('fill', "#2caf49");

		$(".Temperature text").text(temperature+"°C")
	})	
}

function ActionnerMangeoire(){
	$.ajax({
		url: url+'mangeoire',
		type: 'GET'
	})
	.done(function(repas) {
		$(".Nourriture text").text(repas+" repas")
	})	
}

$(document).ready(function() {
	RecupererEtats();	
	$(".Lumiere").click(()=>GererLumiere());
	$(".Nourriture").click(()=>ActionnerMangeoire());
	setInterval(()=>RecupererTemperature(),10000);
});