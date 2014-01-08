DROP FUNCTION sdist;
DROP FUNCTION sarea;
DROP FUNCTION scenter;
DROP FUNCTION scircum;
DROP FUNCTION slength;
DROP FUNCTION sswap;
DROP FUNCTION sstr;
DROP FUNCTION mysql_sphere_version;

DROP FUNCTION srcontainsl;
DROP FUNCTION slcontainsr;
DROP FUNCTION srnotcontainsl;
DROP FUNCTION slnotcontainsr;
DROP FUNCTION soverlaps;
DROP FUNCTION snotoverlaps;
DROP FUNCTION sequal;
DROP FUNCTION snotequal;
DROP FUNCTION stransform;
DROP FUNCTION sinverse;

DROP FUNCTION spoint;
DROP FUNCTION spoint_long;
DROP FUNCTION spoint_lat;
DROP FUNCTION spoint_x;
DROP FUNCTION spoint_y;
DROP FUNCTION spoint_z;
DROP FUNCTION spoint_equal;


DROP FUNCTION strans;
DROP FUNCTION strans_point;
DROP FUNCTION strans_point_inverse;
DROP FUNCTION strans_equal;
DROP FUNCTION strans_equal_neg;
DROP FUNCTION strans_phi;
DROP FUNCTION strans_theta;
DROP FUNCTION strans_psi;
DROP FUNCTION strans_axes;
DROP FUNCTION strans_invert;
DROP FUNCTION strans_zxz;
DROP FUNCTION strans_trans;
DROP FUNCTION strans_trans_inv;


DROP FUNCTION scircle;
DROP FUNCTION scircle_radius;
DROP FUNCTION scircle_equal;
DROP FUNCTION scircle_equal_neg;
DROP FUNCTION scircle_overlap;
DROP FUNCTION scircle_overlap_neg;
DROP FUNCTION scircle_contained_by_circle;
DROP FUNCTION scircle_contained_by_circle_neg;
DROP FUNCTION scircle_contains_circle;
DROP FUNCTION scircle_contains_circle_neg;
DROP FUNCTION spoint_contained_by_circle;
DROP FUNCTION spoint_contained_by_circle_neg;
DROP FUNCTION spoint_contained_by_circle_com;
DROP FUNCTION spoint_contained_by_circle_com_neg;
DROP FUNCTION strans_circle;
DROP FUNCTION strans_circle_inverse;

DROP FUNCTION sline;
DROP FUNCTION smeridian;
DROP FUNCTION sline_beg;
DROP FUNCTION sline_end;
DROP FUNCTION sline_equal;
DROP FUNCTION sline_equal_neg;
DROP FUNCTION sline_turn;
DROP FUNCTION sline_crosses;
DROP FUNCTION sline_crosses_neg;
DROP FUNCTION sline_overlap;
DROP FUNCTION sline_contains_point;
DROP FUNCTION sline_contains_point_com;
DROP FUNCTION sline_contains_point_neg;
DROP FUNCTION sline_contains_point_com_neg;
DROP FUNCTION strans_line;
DROP FUNCTION strans_line_inverse;
DROP FUNCTION sline_overlap_circle;
DROP FUNCTION sline_overlap_circle_com;
DROP FUNCTION sline_overlap_circle_neg;
DROP FUNCTION sline_overlap_circle_com_neg;
DROP FUNCTION scircle_contains_line;
DROP FUNCTION scircle_contains_line_com;
DROP FUNCTION scircle_contains_line_neg;
DROP FUNCTION scircle_contains_line_com_neg;

DROP FUNCTION sellipse;
DROP FUNCTION sellipse_inc;
DROP FUNCTION sellipse_lrad;
DROP FUNCTION sellipse_srad;
DROP FUNCTION sellipse_equal;
DROP FUNCTION sellipse_equal_neg;
DROP FUNCTION sellipse_contains_ellipse;
DROP FUNCTION sellipse_contains_ellipse_neg;
DROP FUNCTION sellipse_contains_ellipse_com;
DROP FUNCTION sellipse_contains_ellipse_com_neg;
DROP FUNCTION sellipse_overlap_ellipse;
DROP FUNCTION sellipse_overlap_ellipse_neg;
DROP FUNCTION sellipse_contains_point;
DROP FUNCTION sellipse_contains_point_neg;
DROP FUNCTION sellipse_contains_point_com;
DROP FUNCTION sellipse_contains_point_com_neg;
DROP FUNCTION sellipse_contains_circle;
DROP FUNCTION sellipse_contains_circle_neg;
DROP FUNCTION sellipse_contains_circle_com;
DROP FUNCTION sellipse_contains_circle_com_neg;
DROP FUNCTION scircle_contains_ellipse;
DROP FUNCTION scircle_contains_ellipse_neg;
DROP FUNCTION scircle_contains_ellipse_com;
DROP FUNCTION scircle_contains_ellipse_com_neg;
DROP FUNCTION sellipse_overlap_circle;
DROP FUNCTION sellipse_overlap_circle_neg;
DROP FUNCTION sellipse_overlap_circle_com;
DROP FUNCTION sellipse_overlap_circle_com_neg;
DROP FUNCTION sellipse_overlap_line;
DROP FUNCTION sellipse_overlap_line_neg;
DROP FUNCTION sellipse_overlap_line_com;
DROP FUNCTION sellipse_overlap_line_com_neg;
DROP FUNCTION sellipse_contains_line;
DROP FUNCTION sellipse_contains_line_neg;
DROP FUNCTION sellipse_contains_line_com;
DROP FUNCTION sellipse_contains_line_com_neg;
DROP FUNCTION strans_ellipse;
DROP FUNCTION strans_ellipse_inverse;

DROP FUNCTION spoly;
DROP FUNCTION spoly_equal;
DROP FUNCTION spoly_equal_neg;
DROP FUNCTION spoly_contains_polygon;
DROP FUNCTION spoly_contains_polygon_neg;
DROP FUNCTION spoly_contains_polygon_com;
DROP FUNCTION spoly_contains_polygon_com_neg;
DROP FUNCTION spoly_overlap_polygon;
DROP FUNCTION spoly_overlap_polygon_neg;
DROP FUNCTION spoly_contains_point;
DROP FUNCTION spoly_contains_point_neg;
DROP FUNCTION spoly_contains_point_com;
DROP FUNCTION spoly_contains_point_com_neg;
DROP FUNCTION spoly_contains_circle;
DROP FUNCTION spoly_contains_circle_neg;
DROP FUNCTION spoly_contains_circle_com;
DROP FUNCTION spoly_contains_circle_com_neg;
DROP FUNCTION scircle_contains_polygon;
DROP FUNCTION scircle_contains_polygon_neg;
DROP FUNCTION scircle_contains_polygon_com;
DROP FUNCTION scircle_contains_polygon_com_neg;
DROP FUNCTION spoly_overlap_circle;
DROP FUNCTION spoly_overlap_circle_neg;
DROP FUNCTION spoly_overlap_circle_com;
DROP FUNCTION spoly_overlap_circle_com_neg;
DROP FUNCTION spoly_contains_line;
DROP FUNCTION spoly_contains_line_neg;
DROP FUNCTION spoly_contains_line_com;
DROP FUNCTION spoly_contains_line_com_neg;
DROP FUNCTION spoly_overlap_line;
DROP FUNCTION spoly_overlap_line_neg;
DROP FUNCTION spoly_overlap_line_com;
DROP FUNCTION spoly_overlap_line_com_neg;
DROP FUNCTION spoly_contains_ellipse;
DROP FUNCTION spoly_contains_ellipse_neg;
DROP FUNCTION spoly_contains_ellipse_com;
DROP FUNCTION spoly_contains_ellipse_com_neg;
DROP FUNCTION sellipse_contains_polygon;
DROP FUNCTION sellipse_contains_polygon_neg;
DROP FUNCTION sellipse_contains_polygon_com;
DROP FUNCTION sellipse_contains_polygon_com_neg;
DROP FUNCTION spoly_overlap_ellipse;
DROP FUNCTION spoly_overlap_ellipse_neg;
DROP FUNCTION spoly_overlap_ellipse_com;
DROP FUNCTION spoly_overlap_ellipse_com_neg;
DROP FUNCTION strans_poly;
DROP FUNCTION strans_poly_inverse;
DROP AGGREGATE FUNCTION spoly_add_point_aggr;
DROP AGGREGATE FUNCTION spoly_aggr;

DROP FUNCTION spath;
DROP FUNCTION spath_equal;
DROP FUNCTION spath_equal_neg;
DROP FUNCTION spath_overlap_path;
DROP FUNCTION spath_overlap_path_neg;
DROP FUNCTION spath_contains_point;
DROP FUNCTION spath_contains_point_neg;
DROP FUNCTION spath_contains_point_com;
DROP FUNCTION spath_contains_point_com_neg;
DROP FUNCTION scircle_contains_path;
DROP FUNCTION scircle_contains_path_neg;
DROP FUNCTION scircle_contains_path_com;
DROP FUNCTION scircle_contains_path_com_neg;
DROP FUNCTION scircle_overlap_path;
DROP FUNCTION scircle_overlap_path_neg;
DROP FUNCTION scircle_overlap_path_com;
DROP FUNCTION scircle_overlap_path_com_neg;
DROP FUNCTION spath_overlap_line;
DROP FUNCTION spath_overlap_line_neg;
DROP FUNCTION spath_overlap_line_com;
DROP FUNCTION spath_overlap_line_com_neg;
DROP FUNCTION sellipse_contains_path;
DROP FUNCTION sellipse_contains_path_neg;
DROP FUNCTION sellipse_contains_path_com;
DROP FUNCTION sellipse_contains_path_com_neg;
DROP FUNCTION sellipse_overlap_path;
DROP FUNCTION sellipse_overlap_path_neg;
DROP FUNCTION sellipse_overlap_path_com;
DROP FUNCTION sellipse_overlap_path_com_neg;
DROP FUNCTION spoly_contains_path;
DROP FUNCTION spoly_contains_path_neg;
DROP FUNCTION spoly_contains_path_com;
DROP FUNCTION spoly_contains_path_com_neg;
DROP FUNCTION spoly_overlap_path;
DROP FUNCTION spoly_overlap_path_neg;
DROP FUNCTION spoly_overlap_path_com;
DROP FUNCTION spoly_overlap_path_com_neg;
DROP FUNCTION strans_path;
DROP FUNCTION strans_path_inverse;
DROP AGGREGATE FUNCTION spath_add_point_aggr;
DROP AGGREGATE FUNCTION spath_aggr;

DROP FUNCTION sbox;
DROP FUNCTION sbox_sw;
DROP FUNCTION sbox_se;
DROP FUNCTION sbox_nw;
DROP FUNCTION sbox_ne;
DROP FUNCTION sbox_equal;
DROP FUNCTION sbox_equal_neg;
DROP FUNCTION sbox_contains_box;
DROP FUNCTION sbox_contains_box_neg;
DROP FUNCTION sbox_contains_box_com;
DROP FUNCTION sbox_contains_box_com_neg;
DROP FUNCTION sbox_overlap_box;
DROP FUNCTION sbox_overlap_box_neg;
DROP FUNCTION sbox_cont_point;
DROP FUNCTION sbox_cont_point_neg;
DROP FUNCTION sbox_cont_point_com;
DROP FUNCTION sbox_cont_point_com_neg;
DROP FUNCTION sbox_contains_circle;
DROP FUNCTION sbox_contains_circle_neg;
DROP FUNCTION sbox_contains_circle_com;
DROP FUNCTION sbox_contains_circle_com_neg;
DROP FUNCTION scircle_contains_box;
DROP FUNCTION scircle_contains_box_neg;
DROP FUNCTION scircle_contains_box_com;
DROP FUNCTION scircle_contains_box_com_neg;
DROP FUNCTION sbox_overlap_circle;
DROP FUNCTION sbox_overlap_circle_neg;
DROP FUNCTION sbox_overlap_circle_com;
DROP FUNCTION sbox_overlap_circle_com_neg;
DROP FUNCTION sbox_contains_line;
DROP FUNCTION sbox_contains_line_neg;
DROP FUNCTION sbox_contains_line_com;
DROP FUNCTION sbox_contains_line_com_neg;
DROP FUNCTION sbox_overlap_line;
DROP FUNCTION sbox_overlap_line_neg;
DROP FUNCTION sbox_overlap_line_com;
DROP FUNCTION sbox_overlap_line_com_neg;
DROP FUNCTION sbox_contains_ellipse;
DROP FUNCTION sbox_contains_ellipse_neg;
DROP FUNCTION sbox_contains_ellipse_com;
DROP FUNCTION sbox_contains_ellipse_com_neg;
DROP FUNCTION sellipse_contains_box;
DROP FUNCTION sellipse_contains_box_neg;
DROP FUNCTION sellipse_contains_box_com;
DROP FUNCTION sellipse_contains_box_com_neg;
DROP FUNCTION sbox_overlap_ellipse;
DROP FUNCTION sbox_overlap_ellipse_neg;
DROP FUNCTION sbox_overlap_ellipse_com;
DROP FUNCTION sbox_overlap_ellipse_com_neg;
DROP FUNCTION sbox_contains_poly;
DROP FUNCTION sbox_contains_poly_neg;
DROP FUNCTION sbox_contains_poly_com;
DROP FUNCTION sbox_contains_poly_com_neg;
DROP FUNCTION spoly_contains_box;
DROP FUNCTION spoly_contains_box_neg;
DROP FUNCTION spoly_contains_box_com;
DROP FUNCTION spoly_contains_box_com_neg;
DROP FUNCTION sbox_overlap_poly;
DROP FUNCTION sbox_overlap_poly_neg;
DROP FUNCTION sbox_overlap_poly_com;
DROP FUNCTION sbox_overlap_poly_com_neg;
DROP FUNCTION sbox_contains_path;
DROP FUNCTION sbox_contains_path_neg;
DROP FUNCTION sbox_contains_path_com;
DROP FUNCTION sbox_contains_path_com_neg;
DROP FUNCTION sbox_overlap_path;
DROP FUNCTION sbox_overlap_path_neg;
DROP FUNCTION sbox_overlap_path_com;
DROP FUNCTION sbox_overlap_path_com_neg;


UNINSTALL PLUGIN sphere;

