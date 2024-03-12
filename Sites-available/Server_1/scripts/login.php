<?php

	$cookies = $_SERVER['HTTP_COOKIE'];
	if (!empty($cookies))
	{
		header("Location: ../pages/hello.html");
		exit ;
	}
	else
		header("Location: ../pages/login_session.html");

	echo "cookie ===  mehdi sekouni" ;
?>
