<html>
<head>
<title>main_credit_card_information</title>
</head>
<body>

<form method="post" action="main.php">
	<input type="submit" value="main ȭ������ ..." name="submit"/><br/>
</form>

<?php
	$dbc = mysqli_connect('127.0.0.1', 'root', 'apmsetup', 'credit_card')
		or die('Error Connecting to MySQL server.');

	$query = "select * from credit_card";
	$result = mysqli_query($dbc, $query)
		or die('Error Querying database.');

	while($row = mysqli_fetch_array($result)){

		$credit_card_name = $row['credit_card_name'];

		$query1 = "SELECT * FROM last_month_record where credit_card_name = '$credit_card_name' ORDER BY month ASC";

		$result1 = mysqli_query($dbc, $query1)
			or die('Error Querying database.');

		echo '<H2><b> �ſ�ī���̸� : ' . $credit_card_name . '</b></H2>';
		echo '<b>����� ���ǹ� : </b>' . $query1 . '<br/><br/>';
		echo '<b>�ſ�ī���̸� �� �����ش��ϴ½���</b><br/>';

		while($row = mysqli_fetch_array($result1)){
			echo $row['credit_card_name'] . ' (' . $row['month'] . ') ' . $row['month_record'] . '<br/>';
		}

	}
	mysqli_close($dbc);
?>

<br/>
<form method="post" action="main.php">
	<input type="submit" value="main ȭ������ ..." name="submit"/><br/>
</form>

</body>
</html>