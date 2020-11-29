<html>
<body>
<?php 

parse_str(getenv('QUERY_STRING'), $_GET);
// echo getenv('QUERY_STRING');
echo $_GET['fork'];

?>
<?php phpinfo(); ?>
</body>
</html>
