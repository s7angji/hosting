<html>
<title>credit_card_insert</title>
<body>

<H1>credit_card_insert</H1>

<form method="post" action="credit_card_DB_insert.php">
	<label for="credit_card_name">�ſ�ī�� �̸�</label>
	<input type="text" id="credit_card_name" name="credit_card_name"/><br/>

	<label for="phone_num">������ ��ȭ��ȣ</label>
	<input type="text" id="phone_num" name="phone_num"/><br/>

	<label for="web">web ������ �ּ�</label>
	<input type="text" id="web" name="web"/><br/>

	<label for="rating">ȸ�� ���</label>
	<input type="text" id="rating" name="rating"/><br/><br/>

	<input type="submit" value="�ſ�ī�� �߰�" name="submit"/>

</form><br/>

<form method="post" action="main_insert.php">
	<input type="submit" value="�������� ..." name="submit"/><br/>
</form>

<form method="post" action="main.php">
	<input type="submit" value="main ȭ������ ..." name="submit"/><br/>
</form>

</body>
</html>