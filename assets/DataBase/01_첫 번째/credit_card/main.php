<html>
<title>main</title>
<body>

<H1>main</H1>

<?php
	echo date('Y�� m�� d�� H�� i��');
	echo '<br/><br/>';
?>

<form method="post" action="main_insert.php">
	<input type="submit" value="INSERT" name="submit"/>
</form>

<form method="post" action="filter_all.php">
	<input type="submit" value="�ſ� ī�� & ���� �׷� ����" name="submit"/>
</form>

<form method="post" action="filter_credit_card.php">
	<input type="submit" value="�ſ� ī�� ����" name="submit"/>
</form>

<form method="post" action="filter_discount_group.php">
	<input type="submit" value="���� �׷� ����" name="submit"/>
</form>

<form method="post" action="check_last_month_record.php">
	<input type="submit" value="���� ���� ��ȸ" name="submit"/>
</form>

<iframe src="http://localhost/main_credit_card_information.php" width="1000" height="120" scrolling="auto"></iframe>
<iframe src="http://localhost/main_all_benefit_information.php" width="1000" height="320" scrolling="auto"></iframe>

</body>
</html>