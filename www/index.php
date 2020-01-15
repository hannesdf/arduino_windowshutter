<!DOCTYPE html>
<html>
<head>
<title>Shutter App</title>
<meta charset="utf-8"/>
</head>
<body>

<h1>Shutter App</h1>


<button type="button" onclick="<?php mailbox('test'); ?>">Click Me!</button>

<?php
function mailbox($msg) {
//  "http://" + host + ":" + port + "/mailbox/" + message);
  $response = file_get_contents('http://localhost/mailbox/' + $msg);
  echo $response;
}
?>
</body>
</html>
