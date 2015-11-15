-- $ mysql -u root -p create whdoc
-- $ mysql -u root -p whdoc < whdoc.mysql

create table loginreq (
	username  varchar(8) NOT NULL,
	cxx       varchar(128),
	ixx       varchar(128),
	cuu       varchar(128),
	primary key (username)
) default charset=latin1;

create table login (
	iuu  varchar(128) NOT NULL,
	uid  int NOT NULL AUTO_INCREMENT,
	primary key (iuu),
	key (uid)
) default charset=latin1;