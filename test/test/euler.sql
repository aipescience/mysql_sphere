SET sphere_outprec=8;

-- checking Euler transformation operators

SELECT sequal(strans('-10d,0d,10d,ZZZ'), strans('-10d,0d,10d,XXX'));
SELECT snotequal(strans('-40d,0d,40d,ZZZ'), strans('-40d,0d,40d,XXX'));

