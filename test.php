<html>
<body>
<?php 

parse_str(getenv('QUERY_STRING'), $_GET);
// echo getenv('QUERY_STRING');
echo $_GET['fork'];
var_dump($_SERVER);

?>
<?php phpinfo(); ?>
</body>
</html>
