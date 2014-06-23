lines = ARGF.each_line.to_a

times = lines[-1].chomp.split(',')

ms_per_frame = times[times.size/2..-1].map do |x|
  (x.to_f / lines.size.to_f) * 1000
end

puts ms_per_frame.join(', ')


