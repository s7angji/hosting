<html>
<title>main</title>
<body>

<H1>main</H1>

<?php
	echo date('Y년 m월 d일 H시 i분');
	echo '<br/><br/>';
?>

<form method="post" action="main_insert.php">
	<input type="submit" value="INSERT" name="submit"/>
</form>

<form method="post" action="filter_all.php">
	<input type="submit" value="신용 카드 & 할인 그룹 필터" name="submit"/>
</form>

<form method="post" action="filter_credit_card.php">
	<input type="submit" value="신용 카드 필터" name="submit"/>
</form>

<form method="post" action="filter_discount_group.php">
	<input type="submit" value="할인 그룹 필터" name="submit"/>
</form>

<form method="post" action="check_last_month_record.php">
	<input type="submit" value="전월 실적 조회" name="submit"/>
</form>

<iframe src="http://localhost/main_credit_card_information.php" width="1000" height="120" scrolling="auto"></iframe>
<iframe src="http://localhost/main_all_benefit_information.php" width="1000" height="320" scrolling="auto"></iframe>

</body>
</html>