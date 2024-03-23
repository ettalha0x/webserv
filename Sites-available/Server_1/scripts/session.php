<?php
	// header("Content-Type: text/html; charset=UTF-8");
	$cookies = $_POST["HTTP_COOKIE"];
	header("Location: ../pages/hello.html");
	if (!empty($cookies))
		exit ;
	$username = $_POST["username"];
	$password = $_POST["password"];
	
	if ($username === "mehdi" && $password === "sekouni")
	{
		session_save_path("../session");
		session_start();
		$session_id = session_id();
		$_SESSION["username"] = $username;
		setcookie("PHPSESSID" , $session_id, time() + (20), "/");
		// setcookie("PHPSESSID" , $session_id, time() + (120), "/mehdi");
	}
	else
	{
		echo "Error: Missing username or npassword in request body.";
		exit(-1);
	}
?>
