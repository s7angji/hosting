<html>
<title>affiliate_benefit_insert</title>
<body>

<H1>affiliate_benefit_insert</H1>

<b>���� �׷� ����</b><br/>

<form method="post" action="affiliate_benefit_DB_insert.php">

	<label for="credit_card_name">�ſ�ī�� �̸�</label>
	<input type="text" id="credit_card_name" name="credit_card_name"/><br/>

	<label for="month_condition">���� ���� ����</label>
	<input type="text" id="month_condition" name="month_condition"/><br/>

	<label for="aff_name">������ �̸�</label>
	<input type="text" id="aff_name" name="aff_name"/><br/>

	<label for="discount_rate">���� ����(������)</label>
	<input type="text" id="discount_rate" name="discount_rate"/><br/><br/>

	<input type="submit" value="������ �߰�" name="submit"/>

</form><br/>

<form method="post" action="main_insert.php">
	<input type="submit" value="�������� ..." name="submit"/><br/>
</form>

<form method="post" action="main.php">
	<input type="submit" value="main ȭ������ ..." name="submit"/><br/>
</form>

</body>
</html>