-- $ mysql -u root -p create whdoc
-- $ mysql -u root -p whdoc < whdoc.mysql

create table loginreq (
	username  varchar(8) NOT NULL,
	cxx       char(36),
	ixx       char(36),
	cuu       char(36),
	primary key (username)
) default charset=latin1;

create table login (
	iuu   char(36) NOT NULL,
	uid   int NOT NULL auto_increment,
	primary key (iuu),
	key (uid)
) default charset=latin1;

create table idcheck (
	id    unsigned int NOT NULL auto_increment,
	uid   int NOT NULL,
	code  char(8) NOT NULL,
	time  datetime NOT NULL,
	primary key (id)
) default charset=latin1;