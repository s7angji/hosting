<html>
<title>credit_card_DB_insert</title>
<body>

<H1>credit_card_DB_insert</H1>

<?php

	$credit_card_name = $_POST['credit_card_name'];
	$phone_num = $_POST['phone_num'];
	$web = $_POST['web'];
	$rating = $_POST['rating'];

	$dbc = mysqli_connect('127.0.0.1', 'root', 'apmsetup', 'credit_card')
	or die('Error Connecting to MySQL server.');

	$query = "INSERT INTO credit_card values ('$credit_card_name', $phone_num, '$web', '$rating')";

	$result = mysqli_query($dbc, $query)
	or die('Error Querying database.');

	echo '신용카드 ' . $credit_card_name . '이(가) 추가되었습니다.</br>';

	mysqli_close($dbc);
?>

<form method="post" action="credit_card_insert.php">
	<input type="submit" value="이전으로 ..." name="submit"/><br/>
</form>

<form method="post" action="main.php">
	<input type="submit" value="main 화면으로 ..." name="submit"/><br/>
</form>

</body>
</html>