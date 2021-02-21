<html>
<title>discount_group_DB_insert</title>
<body>

<H1>discount_group_DB_insert</H1>

<?php

	$discount_group_name = $_POST['discount_group_name'];
	$aff_name = $_POST['aff_name'];


	$dbc = mysqli_connect('127.0.0.1', 'root', 'apmsetup', 'credit_card')
	or die('Error Connecting to MySQL server.');

	$query = "INSERT INTO discount_group values ('$discount_group_name', '$aff_name')";

	$result = mysqli_query($dbc, $query)
	or die('Error Querying database.');

	echo $discount_group_name . '의 가맹점 ' . $aff_name . '이(가) 추가되었습니다.</br></br></br>';

	mysqli_close($dbc);
?>

<form method="post" action="affiliate_benefit_DB_insert.php">

	<label for="credit_card_name">신용카드 이름</label>
	<input type="text" id="credit_card_name" name="credit_card_name"/><br/>

	<label for="month_condition">전월 실적 조건</label>
	<input type="text" id="month_condition" name="month_condition"/><br/>

<?php
	echo '<label for="' . $aff_name . '">가맹점 이름</label>';
	echo '<input type="text" id="' . aff_name . '" name="aff_name" value="' . $aff_name . '"/><br/>';
?>

	<label for="discount_rate">혜택 내용(할인율)</label>
	<input type="text" id="discount_rate" name="discount_rate"/><br/><br/>

	<input type="submit" value="가맹점 추가" name="submit"/>

</form><br/>

<form method="post" action="discount_group_insert.php">
	<input type="submit" value="이전으로 ..." name="submit"/><br/>
</form>

<form method="post" action="main.php">
	<input type="submit" value="main 화면으로 ..." name="submit"/><br/>
</form>

</body>
</html>