<?php
header("Content-Type: text/html; charset=UTF-8");

if ($_SERVER["REQUEST_METHOD"] == "POST" || $_SERVER["REQUEST_METHOD"] == "GET" || $_SERVER["REQUEST_METHOD"] == "DELETE") {
    // Collect form data
    $oops = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Oops! Something Went Wrong</title><style>body {font-family: Arial, sans-serif;text-align: center;margin-top: 50px;}h1 {color: #dc3545;}p {font-size: 18px;margin-top: 20px;}</style></head><body><h1>Oops! Something Went Wrong</h1><p>Sorry, something went wrong while processing your request. Please try again later.</p></body></html>";
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        $name = $_POST['name'];
        $email = $_POST['email'];
        $phone = $_POST['phone'];
        $message = $_POST['message'];

        $thankyou = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Thank You!</title><style>body {font-family: Arial, sans-serif;text-align: center;margin-top: 50px;}h1 {color: #007bff;}p {font-size: 18px;margin-top: 20px;}</style></head><body><h1>Thank You! $name</h1><p>for your message! We will get back to you soon.</p></body></html>";
        echo $thankyou;
        exit;
    } elseif ($_SERVER["REQUEST_METHOD"] == "GET") {
        $name = $_GET['name'];
        $email = $_GET['email'];
        $phone = $_GET['phone'];
        $message = $_GET['message'];
    
        $thankyou = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Thank You!</title><style>body {font-family: Arial, sans-serif;text-align: center;margin-top: 50px;}h1 {color: #007bff;}p {font-size: 18px;margin-top: 20px;}</style></head><body><h1>Thank You! $name</h1><p>for your message! We will get back to you soon.</p></body></html>";
        echo $thankyou;
        exit;
    } elseif ($_SERVER["REQUEST_METHOD"] == "DELETE") {
        $filename = $_GET['filename'];

        if ($filename) {
            if (unlink($filename)) {
                $deleted = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Thank You!</title><style>body {font-family: Arial, sans-serif;text-align: center;margin-top: 50px;}h1 {color: #007bff;}p {font-size: 18px;margin-top: 20px;}</style></head><body><h1>file " + $filename + " deleted succesfully </h1><p>for your message! We will get back to you soon.</p></body></html>";
                echo $deldeletedeted;
                exit;
            } else {
        
                echo $oops;
                exit;
            }
        } else {
    
            echo $oops;
            exit;
        }
    }
} else {
    echo $oops;
    exit;
}
?>
