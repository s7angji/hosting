<html>
<title>filter_credit_card_DB</title>
<body>

<H1>filter_credit_card_DB</H1>

<form method="post" action="filter_credit_card.php">
	<input type="submit" value="이전으로 ..." name="submit"/><br/>
</form>

<form method="post" action="main.php">
	<input type="submit" value="main 화면으로 ..." name="submit"/><br/></br>
</form>

<?php

	$num = $_POST['num'];
	$tmp = array("", "");
	$tmp[0] = $_POST['filter0'];
	$tmp[1] = $_POST['filter1'];
	$tmp[2] = $_POST['filter2'];
	$tmp[3] = $_POST['filter3'];
	$tmp[4] = $_POST['filter4'];
	$tmp[5] = $_POST['filter5'];
	$tmp[6] = $_POST['filter6'];
	$tmp[7] = $_POST['filter7'];


	$dbc = mysqli_connect('127.0.0.1', 'root', 'apmsetup', 'credit_card')
		or die('Error Connecting to MySQL server.');

	$query = "select * from credit_card";
	$result = mysqli_query($dbc, $query)
		or die('Error Querying database.');

	$check = 0;
	while($row = mysqli_fetch_array($result)){

		$credit_card_name = $row['credit_card_name'];

		for($i = 0; $i < 7; $i++){
			if($tmp[$i] == $credit_card_name){
				$check = 1;
				$index = $i;
				break;
			}
		}

		if($check == 0){
			continue;
		}

		echo '<H2><b> 신용카드이름 : ' . $credit_card_name . '</b></H2>';

		$query1 = "SELECT * FROM (affiliate_benefit NATURAL JOIN discount_group) 
			WHERE month_condition < (SELECT month_record 
			FROM (apply_month NATURAL JOIN last_month_record) 
			WHERE credit_card_name = '$credit_card_name') 
			and credit_card_name = '$credit_card_name'";

		$result1 = mysqli_query($dbc, $query1)
				or die('Error Querying database.');

		echo '<b>사용한 질의문 : </b>' . $query1 . '<br/><br/><br/>';

		echo '-----------------------------------------------------------<br/>';
		echo '<b>가맹점이름 신용카드이름 전월실적조건 할인율 할인그룹이름</b><br/>';

		while($row = mysqli_fetch_array($result1)){
			echo $row['aff_name'] . ' (' . $row['credit_card_name'] . ') ' . $row['month_condition'] .
				' (' . $row['discount_rate'] . ') ' . ' (' . $row['discount_group_name'] . ') ' . '<br/>';
		}
		echo '-----------------------------------------------------------<br/><br/><br/>';

		$check = 0;
	}
	mysqli_close($dbc);





?>

<form method="post" action="filter_credit_card.php">
	<br/><input type="submit" value="이전으로 ..." name="submit"/><br/>
</form>

<form method="post" action="main.php">
	<input type="submit" value="main 화면으로 ..." name="submit"/><br/>
</form>

</body>
</html>