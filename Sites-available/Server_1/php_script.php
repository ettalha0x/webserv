<?php
	header("Content-Type: text/html; charset=UTF-8");
	header("Content-Length: 100");

    $name = $_POST["filename"];
    echo "Hello $name from php script !";
?>
