<?php
class _constructTest extends PHPUnit_Framework_TestCase
{
	public function testconstruct()
	{
		$sdb=new Sequoiadb("localhost:11810");
		$this->assertNotEmpty($sdb);
	}
	public function testconstruct2()
	{
		$sdb2=new Sequoiadb("localhost:11810","root","sdbadmin");
		$this->assertNotEmpty($sdb2);
	}
}
?>
