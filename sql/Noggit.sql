CREATE TABLE `UIDs` (
  `UID` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

INSERT INTO `UIDs` (`UID`) VALUES
(5000);

ALTER TABLE `UIDs`
  ADD PRIMARY KEY (`UID`);