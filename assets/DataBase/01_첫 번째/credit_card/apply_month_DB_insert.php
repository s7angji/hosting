<html>
<title>apply_month_DB_insert</title>
<body>

<H1>apply_month_DB_insert</H1>

<?php

	$month = $_POST['month'];

	$dbc = mysqli_connect('127.0.0.1', 'root', 'apmsetup', 'credit_card')
	or die('Error Connecting to MySQL server.');

	$query = "DELETE FROM apply_month";

	$result = mysqli_query($dbc, $query)
	or die('Error Querying database.');

	$query = "INSERT INTO apply_month values ($month)";

	$result = mysqli_query($dbc, $query)
	or die('Error Querying database.');

	echo $month . '월을 적용합니다.</br>';

	mysqli_close($dbc);
?>

<form method="post" action="apply_month_insert.php">
	<input type="submit" value="이전으로 ..." name="submit"/><br/>
</form>

<form method="post" action="main.php">
	<input type="submit" value="main 화면으로 ..." name="submit"/><br/>
</form>

</body>
</html>