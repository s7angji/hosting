<html>
<head>
<title>filter_credit_card</title>
</head>
<body>

<H1>filter_credit_card</H1>

<b>������ �ʴ� �ſ�ī��� Text�ʵ带 ���� �����ϼ���!!!</b><br/><br/>

<?php
	$dbc = mysqli_connect('127.0.0.1', 'root', 'apmsetup', 'credit_card')
		or die('Error Connecting to MySQL server.');

	$query = "select * from credit_card";
	$result = mysqli_query($dbc, $query)
		or die('Error Querying database.');

	$filter = array("", "");
	$i = 0;
	while($row = mysqli_fetch_array($result)){
		$filter[$i] = $row['credit_card_name'];
		$i++;
	}
	mysqli_close($dbc);
?>

<form method="post" action="filter_credit_card_DB.php">
<?php
	echo '<label for="num"> ��ϵ� �ſ�ī���� �� ���� </label>';
	echo '<input type="text" id="num" name="num" value="' . $i . '"/><br/>';

	for($c = 0; $c < $i; $c++){
		echo '<label for="filter' . $c . '">' . $filter[$c] . '</label>';
		echo '<input type="text" id="filter' . $c . '" name="filter' . $c . '" value="' . $filter[$c] . '"/><br/>';
	}
?>
<br/>
	<input type="submit" value="�ſ�ī�� �����Ͽ� ���� ���� ��� ����" name="submit"/>
</form>

<form method="post" action="main.php">
	<input type="submit" value="main ȭ������ ..." name="submit"/><br/>
</form>

</body>
</html>