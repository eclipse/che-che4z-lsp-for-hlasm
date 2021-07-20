/*
 * Copyright (c) 2019 Broadcom.
 * The term "Broadcom" refers to Broadcom Inc. and/or its subsidiaries.
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Broadcom, Inc. - initial API and implementation
 */

 //rules for data definition operand
parser grammar data_def_rules;

dat_op returns [operand_ptr op]
	: data_def {$op = std::make_unique<data_def_operand>(std::move($data_def.value),provider.get_range($data_def.ctx));};

mach_expr_pars returns [mach_expr_ptr e]
	: lpar mach_expr rpar
	{
		$e = std::move($mach_expr.m_e);
	};

dupl_factor_expr_o returns [mach_expr_ptr e]
	: mach_expr_pars
	{
		$e = std::move($mach_expr_pars.e);
	}
	|;


prog_type_and_modifier_p returns [std::string format, mach_expr_list exprs]
	: e=mach_expr_pars data_def_id
	{
		$format = data_definition::expr_placeholder + std::move($data_def_id.value);
		$exprs.push_back(std::move($e.e));
	}
	| e1=mach_expr_pars id1=data_def_id e2=mach_expr_pars id2=data_def_id
	{
		$format = data_definition::expr_placeholder + std::move($id1.value)
			+ data_definition::expr_placeholder + std::move($id2.value);
		$exprs.push_back(std::move($e1.e));
		$exprs.push_back(std::move($e2.e));
	}
	| e1=mach_expr_pars id1=data_def_id e2=mach_expr_pars id2=data_def_id e3=mach_expr_pars id3=data_def_id
	{
		$format = data_definition::expr_placeholder + std::move($id1.value)
			+ data_definition::expr_placeholder + std::move($id2.value)
			+ data_definition::expr_placeholder + std::move($id3.value);

		$exprs.push_back(std::move($e1.e));
		$exprs.push_back(std::move($e2.e));
		$exprs.push_back(std::move($e3.e));
	}
	| ;

nominal_value returns [nominal_value_ptr value]
	: data_def_string
	{
		$value = std::make_unique<nominal_value_string>($data_def_string.value, provider.get_range($data_def_string.ctx->getStart(),$data_def_string.ctx->getStop()));
	}
	| lpar exprs=mach_expr_or_address_comma_c rpar
	{
		$value = std::make_unique<nominal_value_exprs>(std::move($exprs.exprs));
	};

nominal_value_o returns [nominal_value_ptr nominal, mach_expr_ptr e]
	: nominal_value
	{
		$nominal = std::move($nominal_value.value);
	}
	| mach_expr_pars nominal_value
	{
		$nominal = std::move($nominal_value.value);
		$e = std::move($mach_expr_pars.e);
	}
	|;

data_def_address returns [address_nominal addr]
	: disp=mach_expr lpar base=mach_expr rpar
	{
		$addr.base = std::move($base.m_e);
		$addr.displacement = std::move($disp.m_e);
	};

mach_expr_or_address_comma_c returns [expr_or_address_list exprs]
	: e=mach_expr
	{
		$exprs.push_back(std::move($e.m_e));
	}
	| a=data_def_address
	{
		$exprs.push_back(std::move($a.addr));
	}
	| p_list=mach_expr_or_address_comma_c comma e=mach_expr
	{
		$exprs = std::move($p_list.exprs);
		$exprs.push_back(std::move($e.m_e));
	}
	| p_list=mach_expr_or_address_comma_c comma data_def_address
	{
		$exprs = std::move($p_list.exprs);
		$exprs.push_back(std::move($data_def_address.addr));
	}
	;


data_def returns [data_definition value]
	: d_e=dupl_factor_expr_o data_def_id ptm=prog_type_and_modifier_p nominal_value_o
	{
		std::string form = ($d_e.e ? data_definition::expr_placeholder : "")
			+ std::move($data_def_id.value) + std::move($ptm.format)
			+ ($nominal_value_o.e ? data_definition::expr_placeholder : "")
			+ ($nominal_value_o.nominal ? data_definition::nominal_placeholder : "");
		mach_expr_list exprs;
		if($d_e.e)
			exprs.push_back(std::move($d_e.e));

		exprs.insert(exprs.end(), std::make_move_iterator($ptm.exprs.begin()),
				std::make_move_iterator($ptm.exprs.end()));
		
		if($nominal_value_o.e)
			exprs.push_back(std::move($nominal_value_o.e));

		auto begin_range = provider.get_range($d_e.ctx->getStart(),$d_e.ctx->getStop());

		$value = data_definition::create(collector, std::move(form), std::move(exprs), std::move($nominal_value_o.nominal), begin_range.start);
	};

data_def_ch returns [std::string value]
	: IDENTIFIER								{$value = $IDENTIFIER->getText();} 
	| NUM										{$value = $NUM->getText();}
	| ORDSYMBOL									{$value = $ORDSYMBOL->getText();}
	| minus										{$value = "-";}
	| plus										{$value = "+";}
	| DOT										{$value = ".";};

data_def_id returns [std::string value]
	: data_def_ch								{$value = std::move($data_def_ch.value);}
	| tmp=data_def_id data_def_ch				{$tmp.value.append(std::move($data_def_ch.value));  $value = std::move($tmp.value);};


data_def_string returns [std::string value]
	: ap1=(APOSTROPHE|ATTR) string_ch_c ap2=(APOSTROPHE|ATTR)	
	{ 
		$value.append(std::move($string_ch_c.value));
		collector.add_hl_symbol(token_info(provider.get_range($ap1,$ap2),hl_scopes::string)); 
	};