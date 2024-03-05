window.onload = function() {
	let counter = parseInt(sessionStorage.getItem("counter")) || 0;
	counter++;
	sessionStorage.setItem("counter", counter);
	document.getElementById("counter").textContent = "Counter: " + counter;
};