<html>
<title>affiliate_benefit_DB_insert</title>
<body>

<H1>affiliate_benefit_DB_insert</H1>

<?php

	$credit_card_name = $_POST['credit_card_name'];
	$month_condition = $_POST['month_condition'];
	$aff_name = $_POST['aff_name'];
	$discount_rate = $_POST['discount_rate'];

	$dbc = mysqli_connect('127.0.0.1', 'root', 'apmsetup', 'credit_card')
	or die('Error Connecting to MySQL server.');

	$query = "INSERT INTO affiliate_benefit values ('$credit_card_name', $month_condition, '$aff_name', $discount_rate)";

	$result = mysqli_query($dbc, $query)
	or die('Error Querying database.');

	echo $aff_name . '이(가) 추가되었습니다.</br></br>';

	mysqli_close($dbc);
?>

<form method="post" action="affiliate_benefit_insert.php">
	<input type="submit" value="이전으로 ..." name="submit"/><br/>
</form>

<form method="post" action="main.php">
	<input type="submit" value="main 화면으로 ..." name="submit"/><br/>
</form>

</body>
</html>