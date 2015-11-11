
		pos = it->buf->pos;
		last = it->buf->last;
		
		if (state&1 == 0) {
			p1 = memchr(pos, '=', last-pos);
			if (p1) {
				strp = str;
				p1++;
				if (p1 != last)
					it->buf->pos = p1;
				else
					it = it->next;
				state++;
			}
			else
				it = it->next;
		}
		else if (state == 1) {
			p1 = memchr(pos, '&', last-pos);
			if (p1==NULL) p1=last;
			
			n = p1-pos;
			if (strp-str+n > NumStrMaxLen)
				break;
			strncpy(strp, pos, n);
			strp += n;
			
			if (p1 != last) {
				*strp = '\0';
				pd = state==1 ? &d1 : &d2;
				if (sscanf((char*)str, "%lf", pd) == -1)
					break;
				state++;
				if (last-p1-1 > 0)
					it->buf->pos = p1+1;
				else
					it = it->next;
			}
			else
				it = it->next;
		}
		else if (state == 3) {
			
		}