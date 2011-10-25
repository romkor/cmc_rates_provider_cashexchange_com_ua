<?php

function sync_provider_cashexchange_com_ua($currency_code, $target_currencies) {
  $data = cache_get(__FUNCTION__, 'cache');

  if (!$data) {
    $cashexchange_rates = array();
    if (($XML = @simplexml_load_file('http://cashexchange.com.ua/XmlApi.ashx?company=banklviv')) && @count($XML->Element)) {      
      foreach ($XML->Element as $rate) {
        $cashexchange_rates[(string) $rate->Currency] = (string) $rate->Buy;
      }
      // Cache 6 hours.
      cache_set(__FUNCTION__, $cashexchange_rates, 'cache', time() + (3600 * 6));
	  watchdog(
        'cmc_rates_provider_cashexchange_com_ua', 'Rate provider http://cashexchange.com.ua: Successfully fetch rates from @url',
        array('@url' => 'http://cashexchange.com.ua/XmlApi.ashx'),
        WATCHDOG_NOTICE
      );
    }
    else {
      watchdog(
        'cmc_rates_provider_cashexchange_com_ua', 'Rate provider http://cashexchange.com.ua/: Unable to fetch / process the currency data of @url',
        array('@url' => 'http://cashexchange.com.ua/XmlApi.ashx'),
        WATCHDOG_ERROR
      );
    }
  }
  else {
    $cashexchange_rates = $data->data;
  }

  $rates = array();
  foreach ($target_currencies as $target_currency_code) {
    if ($currency_code == 'UAH' && isset($cashexchange_rates[$target_currency_code])) {
      $rates[$target_currency_code] = 1 / $cashexchange_rates[$target_currency_code];
    }
    elseif (isset($cashexchange_rates[$currency_code]) && $target_currency_code == 'UAH') {
      // Reverse rate calculation
      $rates[$target_currency_code] = $cashexchange_rates[$currency_code];
    }
    elseif (isset($cashexchange_rates[$currency_code]) && isset($cashexchange_rates[$target_currency_code])) {
      // Cross rate calculation
      $rates[$target_currency_code] = $cashexchange_rates[$currency_code] / $cashexchange_rates[$target_currency_code];
    }
  }  
  return $rates;  
}


