<html>
<head>
<title>filter_credit_card</title>
</head>
<body>

<H1>filter_credit_card</H1>

<b>원하지 않는 신용카드는 Text필드를 비우고 진행하세요!!!</b><br/><br/>

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
	echo '<label for="num"> 등록된 신용카드의 총 갯수 </label>';
	echo '<input type="text" id="num" name="num" value="' . $i . '"/><br/>';

	for($c = 0; $c < $i; $c++){
		echo '<label for="filter' . $c . '">' . $filter[$c] . '</label>';
		echo '<input type="text" id="filter' . $c . '" name="filter' . $c . '" value="' . $filter[$c] . '"/><br/>';
	}
?>
<br/>
	<input type="submit" value="신용카드 필터하여 할인 혜택 골라 보기" name="submit"/>
</form>

<form method="post" action="main.php">
	<input type="submit" value="main 화면으로 ..." name="submit"/><br/>
</form>

</body>
</html>