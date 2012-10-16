<?php
/**
 * connection to the Facebook graph api to return likes count.
 * 
 * connects to Katy Perry's basic info, using KP as an example 
 * down to the massive amount of likes per minute [not out of music taste :P].
 * Drop this file on your web server then add in the domain & path to your arduino sketch. 
 * 
 * @author Tom Wilderspin 2012
 */

$json_url ="https://graph.facebook.com/katyperry";
$json = file_get_contents($json_url);
$json_output = json_decode($json);
$likes = 0;
if($json_output->likes){
	$likes = $json_output->likes;
}
echo '?='.$likes;


?>