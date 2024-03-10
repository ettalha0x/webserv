<?php

	$cookies = $_SERVER['HTTP_COOKIE'];
	if (!empty($cookies))
	{
		header("Location: hello.html");
		exit ;
	}
	else
		header("Location: login_session.html");

	echo "cookie ===  mehdi sekouni" ;
?>
