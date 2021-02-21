<html>
<title>filter_discount_group_DB</title>
<body>

<H1>filter_discount_group_DB</H1>

<form method="post" action="filter_discount_group.php">
	<input type="submit" value="이전으로 ..." name="submit"/><br/>
</form>

<form method="post" action="main.php">
	<input type="submit" value="main 화면으로 ..." name="submit"/><br/><br/>
</form>

<?php

	$shopping = $_POST['shopping'];
	$refuel = $_POST['refuel'];
	$traffic = $_POST['traffic'];
	$mobile = $_POST['mobile'];
	$beauty = $_POST['beauty'];
	$hospital = $_POST['hospital'];
	$pharmacy = $_POST['pharmacy'];
	$edu = $_POST['edu'];
	$internet = $_POST['internet'];
	$entertainment = $_POST['entertainment'];

	echo '<b>선택된 할인 그룹 : </b>' . $shopping . ' ' . $refuel . ' ' . $traffic . ' ' . $mobile . ' ' . 
		$beauty . ' ' . $hospital . ' ' . $pharmacy . ' ' . $edu . ' ' . $internet . ' ' . $entertainment . '<br/><br/>';

	$filter = array("", "", "", "", "", "", "", "", "", "");

	if($shopping != ""){
		$filter[0] = "and discount_group_name = '$shopping' ";
	}
	if($refuel != ""){
		$filter[1] = "and discount_group_name = '$refuel' ";
	}
	if($traffic != ""){
		$filter[2] = "and discount_group_name = '$traffic' ";
	}
	if($mobile != ""){
		$filter[3] = "and discount_group_name = '$mobile' ";
	}
	if($beauty != ""){
		$filter[4] = "and discount_group_name = '$beauty' ";
	}
	if($hospital != ""){
		$filter[5] = "and discount_group_name = '$hospital' ";
	}
	if($pharmacy != ""){
		$filter[6] = "and discount_group_name = '$pharmacy' ";
	}
	if($edu != ""){
		$filter[7] = "and discount_group_name = '$edu' ";
	}
	if($internet != ""){
		$filter[8] = "and discount_group_name = '$internet' ";
	}
	if($entertainment != ""){
		$filter[9] = "and discount_group_name = '$entertainment' ";
	}


	$dbc = mysqli_connect('127.0.0.1', 'root', 'apmsetup', 'credit_card')
		or die('Error Connecting to MySQL server.');

	$query = "select * from credit_card";
	$result = mysqli_query($dbc, $query)
		or die('Error Querying database.');

	while($row = mysqli_fetch_array($result)){

		$credit_card_name = $row['credit_card_name'];

		echo '<H2><b> 신용카드이름 : ' . $credit_card_name . '</b></H2>';

		for($i = 0; $i <10; $i++){

			if($filter[$i] == ""){
				continue;
			}

			$query1 = "SELECT * FROM (affiliate_benefit NATURAL JOIN discount_group) 
				WHERE month_condition < (SELECT month_record 
				FROM (apply_month NATURAL JOIN last_month_record) 
				WHERE credit_card_name = '$credit_card_name') 
				and credit_card_name = '$credit_card_name'
				$filter[$i]";

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
		}
	}
	mysqli_close($dbc);
?>


<form method="post" action="filter_discount_group.php">
	</br><input type="submit" value="이전으로 ..." name="submit"/><br/>
</form>

<form method="post" action="main.php">
	<input type="submit" value="main 화면으로 ..." name="submit"/><br/>
</form>

</body>
</html>