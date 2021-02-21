<html>
<title>last_month_record_DB_insert</title>
<body>

<H1>last_month_record_DB_insert</H1>

<?php

	$credit_card_name = $_POST['credit_card_name'];
	$month = $_POST['month'];
	$month_record = $_POST['month_record'];

	$dbc = mysqli_connect('127.0.0.1', 'root', 'apmsetup', 'credit_card')
	or die('Error Connecting to MySQL server.');

	$query = "DELETE FROM last_month_record WHERE month = $month and credit_card_name = '$credit_card_name'";

	$result = mysqli_query($dbc, $query)
	or die('Error Querying database.');

	$query = "INSERT INTO last_month_record values ('$credit_card_name', $month, $month_record)";

	$result = mysqli_query($dbc, $query)
	or die('Error Querying database.');

	echo '신용카드 ' . $credit_card_name . '의 ' . $month . '월 실적이 추가되었습니다.</br>';

	mysqli_close($dbc);
?>

<form method="post" action="last_month_record_insert.php">
	<input type="submit" value="이전으로 ..." name="submit"/><br/>
</form>

<form method="post" action="main.php">
	<input type="submit" value="main 화면으로 ..." name="submit"/><br/>
</form>

</body>
</html>