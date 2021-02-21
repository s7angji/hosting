<html>
<head>
<title>main_credit_card_information</title>
</head>
<body>

<?php
	$dbc = mysqli_connect('127.0.0.1', 'root', 'apmsetup', 'credit_card')
		or die('Error Connecting to MySQL server.');

	$query = "select * from credit_card";
	$result = mysqli_query($dbc, $query)
		or die('Error Querying database.');

	while($row = mysqli_fetch_array($result)){

		$credit_card_name = $row['credit_card_name'];

		$query1 = "SELECT * FROM (affiliate_benefit NATURAL JOIN discount_group) 
			WHERE month_condition < (SELECT month_record 
			FROM (apply_month NATURAL JOIN last_month_record) 
			WHERE credit_card_name = '$credit_card_name') 
			and credit_card_name = '$credit_card_name'";

		$result1 = mysqli_query($dbc, $query1)
			or die('Error Querying database.');

		echo '<H2><b> 신용카드이름 : ' . $credit_card_name . '</b></H2>';
		echo '<b>사용한 질의문 : </b>' . $query1 . '<br/><br/>';
		echo '<b>가맹점이름 신용카드이름 전월실적조건 할인율 할인그룹이름</b><br/>';

		while($row = mysqli_fetch_array($result1)){
			echo $row['aff_name'] . ' (' . $row['credit_card_name'] . ') ' . $row['month_condition'] .
				' (' . $row['discount_rate'] . ') ' . ' (' . $row['discount_group_name'] . ') ' . '<br/>';
		}

	}
	mysqli_close($dbc);
?>
</body>
</html>